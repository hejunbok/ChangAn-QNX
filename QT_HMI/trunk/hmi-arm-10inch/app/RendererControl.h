#ifndef RENDERERCONTROL_H
#define RENDERERCONTROL_H

#include <QObject>
#include <QProcess>

class WindowManager;

namespace QnxCar {
    class VideoPlayer;
}

class RendererControl : public QObject
{
    Q_OBJECT

public:
    explicit RendererControl(QnxCar::VideoPlayer *videoPlayer,
                    QObject *parent = 0);

    QnxCar::VideoPlayer *videoPlayer() const;

    Q_PID rendererPid() const;

private Q_SLOTS:
    void handleWindowPosted(Q_PID);
    void handleVideoPlayerChanged();

private:
    /// Moves and resize the window created by the 'mm-renderer' process
    void updateVideoWindow();

    /**
     * Try to retrieve PID of 'mm-renderer' process
     * by starting the console application 'pidin'
     *
     * @return PID of 'mm-renderer' on success, else 0
     */
    Q_PID retrieveRendererPid() const;

    QnxCar::VideoPlayer *m_videoPlayer;

    Q_PID m_rendererPid;
    WindowManager *windowManager;
};

#endif
