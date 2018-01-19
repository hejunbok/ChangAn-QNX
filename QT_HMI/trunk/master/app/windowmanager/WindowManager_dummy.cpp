#include "WindowManager.h"

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class WindowManager::Private
{
public:
    Private()
        : quitRequested(false)
    {
    }

    bool quitRequested;
    QMutex quitRequestedLock;
    QWaitCondition waitForQuit;
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

void registerWindowForProcess(Q_PID, WId)
{
}

void WindowManager::setTransparency(Q_PID, TransparencyMode)
{
}

void WindowManager::setSensitivity(Q_PID, Sensitivities)
{
}

void WindowManager::setSensitivity(const QString &windowGroup, Sensitivities sensitivity)
{
}

bool WindowManager::init()
{
    return true;
}

void WindowManager::run()
{
    bool quitRequested = false;
    while (!quitRequested) {
        QMutexLocker locker( &d->quitRequestedLock );
        if (!d->quitRequested)
            d->waitForQuit.wait( &d->quitRequestedLock );
        quitRequested = d->quitRequested;
    }

    emit quit(0, QString());
}

void WindowManager::setZValueForWindow(Q_PID, WId, int)
{
}

void WindowManager::setZValue(Q_PID, int)
{
}

void WindowManager::setZValue(const QString&, int)
{
}

void WindowManager::setAlignment(Q_PID, Qt::Alignment)
{
}

void WindowManager::setClipSize(Q_PID, const QSize &)
{
}

void WindowManager::setSize(Q_PID, const QSize &)
{
}

void WindowManager::setSourceClipSize(Q_PID, const QSize &)
{
}

void WindowManager::setSourceSize(Q_PID, const QSize &)
{
}

void WindowManager::setClipPosition(Q_PID, const QPoint &)
{
}

void WindowManager::setPosition(Q_PID, const QPoint &)
{
}

void WindowManager::setGeometry(Q_PID, const QRect &)
{
}

void WindowManager::setX(Q_PID, int)
{
}

void WindowManager::setY(Q_PID, int)
{
}

void WindowManager::setVisible(Q_PID, bool)
{
}

void WindowManager::registerWindowForProcess(Q_PID, WId)
{
}

void WindowManager::requestQuit()
{
    QMutexLocker locker(&d->quitRequestedLock);
    d->quitRequested = true;
    d->waitForQuit.wakeAll();
}
