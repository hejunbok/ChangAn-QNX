#include "ResolutionManager.h"

#include <QQmlContext>
#include <qqml.h>

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QSize>
#include <QStringList>

using namespace QnxCarUi;

static ResolutionManager::Resolution parseResolution(const QString &str, bool *ok)
{
    Q_ASSERT(ok);
    *ok = true;
    if (str == QStringLiteral("800x480"))
        return ResolutionManager::Resolution_800x480;
    if (str == QStringLiteral("720p"))
        return ResolutionManager::Resolution_720p;
     if (str == QStringLiteral("1920x1200"))
        return ResolutionManager::Resolution_1920x1200;
     if (str == QStringLiteral("1280x800"))
         return ResolutionManager::Resolution_1280x800;

    *ok = false;
    return ResolutionManager::Resolution_800x480;
}

static qreal resolutionFactorY(ResolutionManager::Resolution r)
{
    switch (r) {
    case ResolutionManager::Resolution_800x480:
        return 1.0;
    case ResolutionManager::Resolution_720p:
        return 1.5;
    case ResolutionManager::Resolution_1920x1200:
        return 2.5;
    case ResolutionManager::Resolution_1280x800:
        return 1.5;
    }

    Q_ASSERT(!"Unexpected ResolutionManager::Resolution value");
    return 1.0;
}

static qreal resolutionFactor(ResolutionManager::Resolution r)
{
    switch (r) {
    case ResolutionManager::Resolution_800x480:
        return 1.0;
    case ResolutionManager::Resolution_720p:
        return 1.6;
    case ResolutionManager::Resolution_1920x1200:
        return 2.4;
    case ResolutionManager::Resolution_1280x800:
        return 1.5;
    }

    Q_ASSERT(!"Unexpected ResolutionManager::Resolution value");
    return 1.0;
}

static ResolutionManager::Resolution determineResolution(const QSize& s)
{
//    if (s.width() < 1280)
//        return ResolutionManager::Resolution_800x480;
//    else
//        return ResolutionManager::Resolution_720p;
//    return ResolutionManager::Resolution_800x480;
//    return ResolutionManager::Resolution_1280x800;
	return ResolutionManager::Resolution_1920x1200;
}

ResolutionManager::ResolutionManager(QObject *parent)
    : QObject(parent)
    , m_resolution(Resolution_1920x1200)
{
    if (const QScreen *const screen = QGuiApplication::primaryScreen()) {
        m_resolution = determineResolution(screen->geometry().size());
    } else {
        qWarning() << "Could not detect resolution, no screen found!";
    }
}

bool ResolutionManager::parseArguments(const QStringList &arguments)
{
    bool resolutionFound = false;
    Q_FOREACH(const QString &arg, arguments) {
        if (arg.startsWith(QStringLiteral("--resolution="))) {
            if (resolutionFound) {
                m_errorString = QObject::tr("Multiple --resolution arguments not supported");
                return false;
            }
            resolutionFound = true;
            const QStringList segs = arg.split(QLatin1Char('='));
            if (segs.size() != 2 || segs[1].isEmpty()) {
                m_errorString = QObject::tr("Invalid --resolution argument");
                return false;
            }

            const QString resStr = segs[1];
            bool ok;
            const ResolutionManager::Resolution r = parseResolution(resStr, &ok);

            if (!ok) {
                m_errorString = QObject::tr("Unknown resolution: '%1'. Supported: '800x480', '720p'").arg(resStr);
                return false;
            }

            m_resolution = r;
        }
    }

    return true;
}

QString ResolutionManager::errorString() const
{
    return m_errorString;
}

ResolutionManager::Resolution ResolutionManager::resolution() const
{
    return m_resolution;
}

void ResolutionManager::setupQmlContext(QQmlContext *context)
{
    Q_ASSERT(context);
    qmlRegisterType<ResolutionManager>("com.qnx.car.ui", 2, 0, "ResolutionManager");
    context->setContextProperty(QStringLiteral("_resolutionFactor"), resolutionFactor());
    context->setContextProperty(QStringLiteral("_resolutionFactorY"), resolutionFactorY());
    context->setContextProperty(QStringLiteral("_resolutionManager"), this);
}

int ResolutionManager::heightForResolution() const
{
    return sizeForResolution().height();
}

int ResolutionManager::widthForResolution() const
{
    return sizeForResolution().width();
}

QSize ResolutionManager::sizeForResolution() const
{
    switch (m_resolution) {
    case Resolution_800x480:
        return QSize(800, 480);
    case Resolution_720p:
        return QSize(1280, 720);
    case Resolution_1920x1200:
        return QSize(1200, 1920);
    case Resolution_1280x800:
        return QSize(1280, 800);
    }

    return QSize();
}

QRect ResolutionManager::mapRect(const QRect &rect) const
{
    const qreal rf = resolutionFactor();
    return QRect(rect.topLeft() * rf, mapSize(rect.size()));
}

QPoint ResolutionManager::mapPoint(const QPoint &p) const
{
    const qreal rf = resolutionFactor();
    return p * rf;
}

QSize ResolutionManager::mapSize(const QSize &size) const
{
    const qreal rf = resolutionFactor();
    return size * rf;
}

int ResolutionManager::mapPixels(int px) const
{
    return qRound(resolutionFactor() * px);
}

qreal ResolutionManager::resolutionFactorY() const
{
    return ::resolutionFactorY(m_resolution);
}

qreal ResolutionManager::resolutionFactor() const
{
    return ::resolutionFactor(m_resolution);
}

int ResolutionManager::defaultPixelSize() const
{
    return qRound(14 * resolutionFactor());
}

QRect ResolutionManager::externalApplicationGeometry() const
{
    //the geometry of external application is the screen minus the status bar
    //and navigator
    switch (m_resolution) {
    case Resolution_800x480:
        return QRect(0, 25+680, 768, 595); //statusbar height: 25, navigator: 60
    case Resolution_720p:
        return QRect(0, 40, 1280, 584); //status bar: 40, navigator: 96
    case Resolution_1920x1200:
        //return QRect(0, 120, 1200, 1599); //status bar: 120, navigator: 201
        return QRect(0, 201, 1200, 800); //status bar: 120, navigator: 201
    case Resolution_1280x800:
        return QRect(0, 201, 1280, 505);
    }

    return QRect();
}
