#ifndef CAMERAOVERLAYWINDOW_H
#define CAMERAOVERLAYWINDOW_H

#include <QtGui>

/**
 * @brief basic raster window serves as camera overlay
 * This is basic raster windows, extends Qwindow and serves as rearview camera overlay.
 * Camera will join this window's group when it is available
 */
class CameraOverlayWindow : public QWindow
{
    Q_OBJECT
public:
    explicit CameraOverlayWindow(QWindow *parent = 0);

public slots:
    /**
     * @brief renders background and content
     * Function draws black background and white text
     */
    void renderNow();

protected:
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void exposeEvent(QExposeEvent *event);

private:
    QBackingStore *m_backingStore;
    bool m_update_pending;
};
#endif // CAMERAOVERLAYWINDOW_H
