#include "WindowManager.h"
#include "Exception.h"
#include "Optional.h"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QSet>

#include <screen/screen.h>

#include <cerrno>

static inline void toArray(const QSize &size, int a[2])
{
    a[0] = size.width();
    a[1] = size.height();
}

static inline void toArray(const QPoint &point, int a[2])
{
    a[0] = point.x();
    a[1] = point.y();
}

static inline void throwIfSet(int rc, const QString& message)
{
    if (rc != 0)
        throw Exception(QObject::tr("%1: %2 (%3)").arg(message, QString::fromLocal8Bit(strerror(errno)), QString::number(errno)));
}

static inline void throwIfNotIn(int rc, const QVector<int> &expectedErrnos, const QString &message)
{
    if (rc != 0 && !expectedErrnos.contains(errno))
        throw Exception(QObject::tr("%1: %2 (%3)").arg(message, QString::fromLocal8Bit(strerror(errno)), QString::number(errno)));
}

struct Window
{
    explicit Window(screen_window_t w = 0, bool igp = false)
        : window(w)
        , isGroupParent(igp)
    {
    }

    screen_window_t window;
    bool isGroupParent;
};

struct WinHandleMatches
{
    const screen_window_t m_winhandle;

    explicit WinHandleMatches(screen_window_t winhandle)
        : m_winhandle(winhandle)
    {
    }

    bool operator()(const Window win) const
    {
        return (win.window == m_winhandle);
    }
};

static inline void setWindowClipPosition(screen_window_t window, const QPoint &point)
{
    int val[2];
    toArray(point, val);
    const int rc2 = screen_set_window_property_iv(window,  SCREEN_PROPERTY_CLIP_POSITION, val);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_CLIP_POSITION)"));
}

static inline void setWindowPosition(screen_window_t window, const QPoint &point)
{
    int val[2];
    toArray(point, val);
    const int rc2 = screen_set_window_property_iv(window, SCREEN_PROPERTY_POSITION, val);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_POSITION)"));
}

static inline void setWindowClipSize(screen_window_t window, const QSize &size)
{
    int val[2];
    toArray(size, val);
    const int rc2 = screen_set_window_property_iv(window,  SCREEN_PROPERTY_CLIP_SIZE, val);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_CLIP_SIZE)"));
}

static inline void setWindowSourceClipSize(screen_window_t window, const QSize &size)
{
    int val[2];
    toArray(size, val);
    const int rc2 = screen_set_window_property_iv(window,  SCREEN_PROPERTY_SOURCE_CLIP_SIZE, val);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_SOURCE_CLIP_SIZE)"));
}

static inline void setWindowSourceSize(screen_window_t window, const QSize &size)
{
    int val[2];
    toArray(size, val);
    const int rc2 = screen_set_window_property_iv(window, SCREEN_PROPERTY_SOURCE_SIZE, val);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_SOURCE_SIZE)"));
}

static inline void setWindowSize(screen_window_t window, const QSize &size)
{
    int val[2];
    toArray(size, val);
    const int rc2 = screen_set_window_property_iv(window, SCREEN_PROPERTY_SIZE, val);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_SIZE)"));
}

static inline void setWindowTouchSensitivity(screen_window_t window, const int mask )
{
    const int rc2 = screen_set_window_property_iv(window, SCREEN_PROPERTY_SENSITIVITY, &mask);
    throwIfSet(rc2, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_SENSITIVITY)"));
}

static inline void setWindowAlignment(screen_window_t window, Qt::Alignment alignment)
{
    int posX = 0;
    int posY = 0;

    int val[2];

    if ((alignment & Qt::AlignRight) || (alignment & Qt::AlignBottom)) {
        const int rc = screen_get_window_property_iv(window, SCREEN_PROPERTY_SIZE, val);
        throwIfSet(rc, QStringLiteral("screen_get_window_property_iv(SCREEN_PROPERTY_SIZE)"));

        int width = val[0];
        int height = val[1];

        QScreen *screen = QGuiApplication::primaryScreen();

        if (alignment & Qt::AlignRight)
            posX = screen->size().width() - width;

        if (alignment & Qt::AlignBottom)
            posY = screen->size().height() - height;
    }

    setWindowPosition(window, QPoint(posX, posY));
}

static inline void setWindowZValue(screen_window_t window, int zvalue)
{
    const int rc = screen_set_window_property_iv(window, SCREEN_PROPERTY_ZORDER, &zvalue);
    throwIfNotIn(rc, QVector<int>() << ENOENT, QStringLiteral("screen_set_event_property_iv(SCREEN_PROPERTY_ZORDER)"));
}

static inline void setWindowVisible(screen_window_t window, bool visible)
{
    const int iv = visible ? 1 : 0;

    const int rc = screen_set_window_property_iv(window, SCREEN_PROPERTY_VISIBLE, &iv);
    throwIfNotIn(rc, QVector<int>() << ENOENT, QStringLiteral("screen_set_event_property_iv(SCREEN_PROPERTY_VISIBLE) failed."));
}

/// Properties for a window
struct WindowPropertyData
{
    Optional<QPoint> pos;
    Optional<QPoint> clipPos;
    Optional<QSize> size;
    Optional<QSize> clipSize;
    Optional<QSize> sourceSize;
    Optional<QSize> sourceClipSize;
    Optional<Qt::Alignment> alignment;
    Optional<int> zvalue;
    Optional<bool> visible;
    Optional<int> sensitivity;
    Optional<WindowManager::TransparencyMode> transparencyMode;
};

struct ProcessPropertyData
{
    /// Default window properties used if nothing special was set
    WindowPropertyData defaults;
    /// Window-specific properties
    QMap<screen_window_t, WindowPropertyData> windowSpecific;
 };


QDebug operator<<(QDebug dbg, const WindowPropertyData &data)
{
    dbg.nospace() <<
        "[pos=" << data.pos <<
        ", size=" << data.size <<
        ", clipPos=" << data.clipPos <<
        ", clipSize=" << data.clipSize <<
        ", alignment=" << data.alignment <<
        ", transparency=" << data.transparencyMode <<
        ", zvalue=" << data.zvalue <<
        ", visible=" << data.visible <<
        "]";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const ProcessPropertyData &data)
{
    dbg.nospace() << "defaults: " << data.defaults;
    QMap<screen_window_t, WindowPropertyData>::ConstIterator it = data.windowSpecific.constBegin();
    for (; it != data.windowSpecific.constEnd(); ++it) {
        dbg.nospace() << "window: " << it.key() << " " << it.value();
    }
    return dbg.space();
}

class WindowManager::Private
{
public:
    Private()
        : quitRequested(false)
        , windowOrderDirty(false)
        , windowManagerContext(0)
    {
        qRegisterMetaType<Q_PID>("Q_PID");
    }

    ~Private()
    {
        if (windowManagerContext)
            screen_destroy_context(windowManagerContext);
    }

    void processLocalRequests(screen_context_t context);
    void processLocalRequestsForPid(Q_PID pid);

    void markDirty(Q_PID pid);

    bool quitRequested;
    QMutex quitRequestedLock; //should be atomic, i guess

    QMap<Q_PID, ProcessPropertyData> windowProperties;

    /// set of dirty PIDs, will be processed in processLocalRequests function
    QVector<Q_PID> updateQueue;

    QMutex windowHandlingLock;
    bool windowOrderDirty;
    QHash<Q_PID, QVector<Window> > windowsByProcess;
    QHash<QString, Window> windowsByWindowGroup;

    screen_context_t windowManagerContext;

    inline bool getQuitRequested()
    {
        QMutexLocker locker(&quitRequestedLock);
        return quitRequested;
    }
};

WindowManager::WindowManager(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

WindowManager::~WindowManager()
{
    delete d;
}

struct EventGuard
{
    explicit EventGuard(screen_event_t ev)
        : m_event(ev)
    {
    }

    ~EventGuard()
    {
        screen_destroy_event(m_event);
    }

    screen_event_t m_event;
};

struct ZOrderAndWindow
{
    int zorder;
    screen_window_t window;

    bool operator<(const ZOrderAndWindow &other)
    {
        if (zorder != other.zorder)
            return zorder < other.zorder;
        else
            return window < other.window;
    }
};

bool WindowManager::init()
{
    screen_context_t screen_ctx;

    const int rc = screen_create_context(&screen_ctx, SCREEN_WINDOW_MANAGER_CONTEXT);
    if (rc != 0)
        return false;

    d->windowManagerContext = screen_ctx;
    return true;
}

static int mapTransparencyEnums(WindowManager::TransparencyMode mode)
{
    switch (mode) {
    case WindowManager::Source:
        return SCREEN_TRANSPARENCY_SOURCE;
    case WindowManager::Test:
        return SCREEN_TRANSPARENCY_TEST;
    case WindowManager::SourceColor:
        return SCREEN_TRANSPARENCY_SOURCE_COLOR;
    case WindowManager::SourceOver:
        return SCREEN_TRANSPARENCY_SOURCE_OVER;
    case WindowManager::NoTransparency:
        return SCREEN_TRANSPARENCY_NONE;
    case WindowManager::Discard:
        return SCREEN_TRANSPARENCY_DISCARD;
    }

    Q_ASSERT( !"unexpected transparency value" );

    return SCREEN_TRANSPARENCY_NONE;
}

void WindowManager::Private::processLocalRequests(screen_context_t context)
{
    QMutexLocker locker(&windowHandlingLock);
    if (updateQueue.isEmpty())
        return;

    Q_FOREACH(Q_PID pid, updateQueue) {
        processLocalRequestsForPid(pid);
    }
    updateQueue.clear();

    const int rc = screen_flush_context(context, SCREEN_WAIT_IDLE);
    throwIfSet(rc, QStringLiteral("screen_flush_context"));
}

void WindowManager::Private::processLocalRequestsForPid(Q_PID pid)
{
    const ProcessPropertyData data = windowProperties[pid];

    Q_FOREACH(Window window, windowsByProcess.value(pid)) {
        const QMap<screen_window_t, WindowPropertyData>::ConstIterator winDataIt = data.windowSpecific.constFind(window.window);
        WindowPropertyData winData;
        if (winDataIt != data.windowSpecific.constEnd()) {
            winData = *winDataIt;
        }
        const Optional<int> sensitivity = winData.sensitivity.isSet() ? winData.sensitivity : data.defaults.sensitivity;
        if (sensitivity.isSet()) {
            setWindowTouchSensitivity(window.window, sensitivity.value());
        }
        const Optional<bool> visible = winData.visible.isSet() ? winData.visible : data.defaults.visible;
        if (visible.isSet()) {
            setWindowVisible(window.window, visible.value());
        }
        const Optional<TransparencyMode> transparencyMode = winData.transparencyMode.isSet() ? winData.transparencyMode : data.defaults.transparencyMode;
        if (transparencyMode.isSet()) {
            const int transparency = mapTransparencyEnums(transparencyMode.value());
            const int rc = screen_set_window_property_iv(window.window, SCREEN_PROPERTY_TRANSPARENCY, &transparency);
            throwIfNotIn(rc, QVector<int>() << EOK << ENOENT, QStringLiteral("screen_set_event_property_iv(SCREEN_PROPERTY_TRANSPARENCY)"));
        }
        const Optional<QPoint> clipPos = winData.clipPos.isSet() ? winData.clipPos : data.defaults.clipPos;
        if (clipPos.isSet()) {
            setWindowClipPosition(window.window, clipPos.value());
        }
        const Optional<QPoint> pos = winData.pos.isSet() ? winData.pos : data.defaults.pos;
        if (pos.isSet()) {
            setWindowPosition(window.window, pos.value());
        }
        const Optional<QSize> clipSize = winData.clipSize.isSet() ? winData.clipSize : data.defaults.clipSize;
        if (clipSize.isSet()) {
            setWindowClipSize(window.window, clipSize.value());
        }
        const Optional<QSize> sourceClipSize = winData.sourceClipSize.isSet() ? winData.sourceClipSize : data.defaults.sourceClipSize;
        if (sourceClipSize.isSet()) {
            setWindowSourceClipSize(window.window, sourceClipSize.value());
        }
        const Optional<QSize> size = winData.size.isSet() ? winData.size : data.defaults.size;
        if (size.isSet()) {
            setWindowSize(window.window, size.value());
        }
        const Optional<QSize> sourceSize = winData.sourceSize.isSet() ? winData.sourceSize : data.defaults.sourceSize;
        if (sourceSize.isSet()) {
            setWindowSourceSize(window.window, sourceSize.value());
        }
        const Optional<Qt::Alignment> alignment = winData.alignment.isSet() ? winData.alignment : data.defaults.alignment;
        if (alignment.isSet()) {
            setWindowAlignment(window.window, alignment.value());
        }
        const Optional<int> zvalue = winData.zvalue.isSet() ? winData.zvalue : data.defaults.zvalue;
        if (zvalue.isSet()) {
            setWindowZValue(window.window, zvalue.value());
        }
    }
}

void WindowManager::Private::markDirty(Q_PID pid)
{
    if (updateQueue.contains(pid))
        return;

    updateQueue << pid;
}

static bool get_window_and_pid(screen_event_t event, screen_window_t* winHandle, int* pid)
{
    int rc = screen_get_event_property_pv(event, SCREEN_PROPERTY_WINDOW, (void **)winHandle);

    throwIfSet(rc, QStringLiteral("screen_get_event_property_pv(SCREEN_PROPERTY_WINDOW)"));

    rc = screen_get_window_property_iv(*winHandle, SCREEN_PROPERTY_OWNER_PID, pid);
    throwIfNotIn(rc, QVector<int>() << EOK << EINVAL << -1, QStringLiteral("screen_set_window_property_iv(SCREEN_PROPERTY_OWNER_PID)"));

    return (rc == 0);
}

static QString get_window_group_name(const screen_window_t &winHandle)
{
    QByteArray name(256, 0);
    int rc = screen_get_window_property_cv(winHandle, SCREEN_PROPERTY_GROUP, name.size(), name.data());
    if (rc != 0)
        return QString();

    return QLatin1String(name);
}

void WindowManager::run()
{
    if (!d->windowManagerContext) {
        qDebug() << "No window manager context, abort";
        emit quit(Error, QObject::tr("Initialization failed"));
        return;
    }

    try {
        screen_event_t screen_ev;
        int rc = screen_create_event(&screen_ev);
        throwIfSet(rc, QStringLiteral("screen_create_event"));
        const EventGuard eventGuard(screen_ev);

        bool quitRequested = false;
        while (!quitRequested) {
            d->processLocalRequests(d->windowManagerContext);
            //Set timeout to 100ms
            rc = screen_get_event(d->windowManagerContext, screen_ev, 100000000);

            throwIfSet(rc, QStringLiteral("Could not retrieve event"));

            int type = 0;
            rc = screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TYPE, &type);
            throwIfSet(rc, QStringLiteral("screen_get_event_property_iv(SCREEN_PROPERTY_TYPE)"));

            //update quit flag
            quitRequested = d->getQuitRequested();

            if (type == 0)
                continue;

            switch(type) {
                case SCREEN_EVENT_CREATE:
                {
                    screen_window_t winHandle;

                    int pid = -1;
                    if (get_window_and_pid(screen_ev, &winHandle, &pid)) {
                    }
                    emit windowCreated(pid);
                }
                break;
                case SCREEN_EVENT_POST:
                {
                    screen_window_t winHandle;
                    int pid = -1;
                    if (get_window_and_pid(screen_ev, &winHandle, &pid)) {
                        const QString windowGroup = get_window_group_name(winHandle);

                        {
                            QMutexLocker lock(&d->windowHandlingLock);
                            const Window window(winHandle, /*isGroupParent=*/true);
                            d->windowsByProcess[Q_PID(pid)].append(window);
                            d->windowsByWindowGroup[windowGroup] = window;
                            d->windowOrderDirty = true;
                            d->markDirty(pid);
                        }
                        emit windowPosted(pid);
                        emit windowGroupPosted(windowGroup, pid);
                    }
                }
                break;
                case SCREEN_EVENT_CLOSE:
                {
                    screen_window_t winHandle;
                    int pid = -1;
                    if (get_window_and_pid(screen_ev, &winHandle, &pid)) {
                        const QString windowGroup = get_window_group_name(winHandle);
                        QMutexLocker lock(&d->windowHandlingLock);
                        QHash<Q_PID, QVector<Window> >::Iterator it = d->windowsByProcess.find(Q_PID(pid));
                        if (it != d->windowsByProcess.constEnd()) {
                            it->erase(std::remove_if(it->begin(), it->end(), WinHandleMatches(winHandle)), it->end());
                            if (it->isEmpty())
                                d->windowsByProcess.erase(it);
                        }
                        d->windowProperties.remove(pid);
                        d->windowsByWindowGroup.remove(windowGroup);
                        d->windowOrderDirty = true;
                        emit windowClosed(pid);
                    }
                }
                break;
            default:
                qDebug() << "Unhandled screen event from type: " << type;
                break;
            }
        }
    } catch (const Exception& e) {
        qCritical() << "ABORTING WINDOW MANAGEMENT due to error: " << e.message();
        emit quit(Error, e.message());
        return;
    }

    emit quit(NoError, QString());
}

void WindowManager::requestQuit()
{
    QMutexLocker locker(&d->quitRequestedLock);
    d->quitRequested = true;
}

void WindowManager::setVisible(Q_PID pid, bool visible)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.visible = visible;
    Q_FOREACH(const Window &window, d->windowsByProcess.value(pid)) {
        try {
            setWindowVisible(window.window, visible);
        } catch (const Exception &e) {
            qWarning() << e.message();
        }
    }
    const int rc = screen_flush_context(d->windowManagerContext, SCREEN_WAIT_IDLE);
    if (rc != 0)
        qWarning() << "screen_flush_context returned unexpected:" << rc;
}

void WindowManager::setZValueForWindow(Q_PID pid, WId window, int zvalue)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].windowSpecific[reinterpret_cast<screen_window_t>(window)].zvalue = zvalue;
    d->markDirty(pid);
}

void WindowManager::setZValue(Q_PID pid, int zvalue)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.zvalue = zvalue;
    d->markDirty(pid);
}

void WindowManager::setZValue(const QString &windowGroup, int zvalue)
{
    if (d->windowsByWindowGroup.contains(windowGroup)) {
        setWindowZValue(d->windowsByWindowGroup.value(windowGroup).window, zvalue);
        const int rc = screen_flush_context(d->windowManagerContext, SCREEN_WAIT_IDLE);
        throwIfSet(rc, QStringLiteral("screen_flush_context"));
    }
}


void WindowManager::setAlignment(Q_PID pid, Qt::Alignment alignment)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.alignment = alignment;
    d->markDirty(pid);
}

void WindowManager::setClipSize(Q_PID pid, const QSize &size)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.clipSize = size;
    d->markDirty(pid);
}

void WindowManager::setSize(Q_PID pid, const QSize &size)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.size = size;
    d->markDirty(pid);
}

void WindowManager::setSourceClipSize(Q_PID pid, const QSize &size)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.sourceClipSize = size;
    d->markDirty(pid);
}

void WindowManager::setSourceSize(Q_PID pid, const QSize &size)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.sourceSize = size;
    d->markDirty(pid);
}

void WindowManager::setPosition(Q_PID pid, const QPoint &pos)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.pos = pos;
    d->markDirty(pid);
}

void WindowManager::setGeometry(Q_PID pid, const QRect &geometry)
{
    QMutexLocker locker(&d->windowHandlingLock);
    ProcessPropertyData& winProperties = d->windowProperties[pid];
    winProperties.defaults.pos = geometry.topLeft();
    winProperties.defaults.size = geometry.size();
    d->markDirty(pid);
}

void WindowManager::setClipPosition(Q_PID pid, const QPoint &clipPos)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.clipPos = clipPos;
    d->markDirty(pid);
}

void WindowManager::setX(Q_PID pid, int x)
{
    QMutexLocker locker(&d->windowHandlingLock);
    ProcessPropertyData& props = d->windowProperties[pid];
    QPoint pos = props.defaults.pos.isSet() ? props.defaults.pos.value() : QPoint(0, 0);
    pos.setX(x);
    props.defaults.pos = pos;
    d->markDirty(pid);
}

void WindowManager::setY(Q_PID pid, int y)
{
    QMutexLocker locker(&d->windowHandlingLock);
    ProcessPropertyData& props = d->windowProperties[pid];
    QPoint pos = props.defaults.pos.isSet() ? props.defaults.pos.value() : QPoint(0, 0);
    pos.setY(y);
    props.defaults.pos = pos;
    d->markDirty(pid);
}

void WindowManager::setSensitivity(Q_PID pid, Sensitivities sensitivity)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.sensitivity = sensitivity;
    d->markDirty(pid);
}

void WindowManager::setSensitivity(const QString &windowGroup, Sensitivities sensitivity)
{
    if (d->windowsByWindowGroup.contains(windowGroup)) {
        setWindowTouchSensitivity(d->windowsByWindowGroup.value(windowGroup).window, sensitivity);
        const int rc = screen_flush_context(d->windowManagerContext, SCREEN_WAIT_IDLE);
        throwIfSet(rc, QStringLiteral("screen_flush_context"));
    }
}

void WindowManager::registerWindowForProcess(Q_PID pid, WId winid)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowsByProcess[pid].append(Window(reinterpret_cast<screen_window_t>(winid), false));
    d->windowOrderDirty = true;
}

void WindowManager::setTransparency(Q_PID pid, TransparencyMode mode)
{
    QMutexLocker locker(&d->windowHandlingLock);
    d->windowProperties[pid].defaults.transparencyMode = mode;
    d->markDirty(pid);
}
