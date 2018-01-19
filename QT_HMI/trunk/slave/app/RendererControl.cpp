#include "RendererControl.h"

#include "services/VideoPlayer.h"

#include "windowmanager/WindowManager.h"
#include "util/Util.h"
#include "ApplicationManager.h"
#include "Core.h"

#include <QProcess>

static const int ZORDER_SHOW = 20;
static const int ZORDER_HIDE = -1;

RendererControl::RendererControl(QnxCar::VideoPlayer *videoPlayer,
                                 QObject *parent)
    : QObject(parent)
    , m_videoPlayer(videoPlayer)
{
    Q_ASSERT(videoPlayer);

    connect(Core::self()->windowManager(), SIGNAL(windowPosted(Q_PID)), SLOT(handleWindowPosted(Q_PID)));
    connect(videoPlayer, SIGNAL(viewPortChanged(QRect)), SLOT(handleVideoPlayerChanged()));
    connect(videoPlayer, SIGNAL(stateChanged(QnxCar::VideoPlayer::State)), SLOT(handleVideoPlayerChanged()));
    connect(videoPlayer, &QnxCar::VideoPlayer::videoPixelWidthChanged, this, &RendererControl::updateVideoWindow);
    connect(videoPlayer, &QnxCar::VideoPlayer::videoPixelHeightChanged, this, &RendererControl::updateVideoWindow);

    // try to retrieve the PID of the
    m_rendererPid = retrieveRendererPid();
    if (!m_rendererPid) {
        qWarning() << Q_FUNC_INFO << "Failed to retrieve PID of mm-renderer, VideoPlayer will not function!";
    } else {
        car2_debug() << "Found mm-renderer PID:" << m_rendererPid;
        Core::self()->applicationManager()->addToUnmanagedProcesses(m_rendererPid);
    }
}

Q_PID RendererControl::rendererPid() const
{
    return m_rendererPid;
}

Q_PID RendererControl::retrieveRendererPid() const
{
#ifdef CAR2_HOST_BUILD
    return 0;
#else
    const QString program = QStringLiteral("pidin");
    QStringList arguments;
    arguments << QStringLiteral("-P") << QStringLiteral("mm-renderer");
    arguments << QStringLiteral("-F") << QStringLiteral("%a");

    QProcess process;
    process.start(program, arguments);
    bool success = process.waitForFinished(3000);
    if (!success)
        return 0;

    const QByteArray result = process.readAllStandardOutput();
    const QList<QByteArray> lines = result.split('\n');
    // lines should look like: ("     pid", " 1679413", "")
    if (lines.size() != 3)
        return 0;

    return lines[1].toInt();
#endif
}

QnxCar::VideoPlayer *RendererControl::videoPlayer() const
{
    return m_videoPlayer;
}

void RendererControl::handleWindowPosted(Q_PID pid)
{
    if (!m_rendererPid)
        return;

    if (pid == m_rendererPid) {
        car2_debug() << "Startup of video renderer window catched, applying viewport constraints";
        updateVideoWindow();
    }
}

void RendererControl::handleVideoPlayerChanged()
{
    updateVideoWindow();
}

// Returns the largest possible rectangle centered inside containingRect with the aspect ratio defined by videoWidth and videoHeight
static QRect calculateActualViewport(const QRect &containingRect, int videoWidth, int videoHeight)
{
    const QSize fitSize = QSize(videoWidth, videoHeight).scaled(containingRect.size(), Qt::KeepAspectRatio);
    const int xOffset = (containingRect.width() - fitSize.width()) / 2;
    const int yOffset = (containingRect.height() - fitSize.height()) / 2;
    return QRect(containingRect.topLeft() + QPoint(xOffset, yOffset), fitSize);
}

void RendererControl::updateVideoWindow()
{
    Q_ASSERT(m_videoPlayer);

    const Q_PID pid = m_rendererPid;
    if (!pid)
        return;

    const QnxCar::VideoPlayer::State state = m_videoPlayer->state();
    const QRect containingRect = m_videoPlayer->viewPort();
    //The actually wanted aspect ratio seems to be the ratio of physical and pixel values multiplied
    //see QCARTWO-4367
    const int pixelWidth = m_videoPlayer->videoPixelWidth();
    const int physicalWidth = m_videoPlayer->videoPhysicalWidth();
    const int pixelHeight = m_videoPlayer->videoPixelHeight();
    const int physicalHeight = m_videoPlayer->videoPhysicalHeight();
    const int videoWidth = pixelWidth * physicalWidth;
    const int videoHeight = pixelHeight * physicalHeight;
    const QRect viewPort = calculateActualViewport(containingRect, videoWidth, videoHeight);
    car2_debug() << "State:" << (state == QnxCar::VideoPlayer::PlayingState ? "Playing" : "Not playing")
                 << "- Actual viewport:" << viewPort << "inside rect" << containingRect << "video:" << videoWidth << "x" << videoHeight;
    WindowManager *windowManager = Core::self()->windowManager();
    if (state == QnxCar::VideoPlayer::PlayingState && viewPort.isValid()) {
        windowManager->setPosition(pid, viewPort.topLeft());
        windowManager->setSize(pid, viewPort.size());
        windowManager->setZValue(pid, ZORDER_SHOW);
    } else {
        windowManager->setZValue(pid, ZORDER_HIDE);
    }
}
