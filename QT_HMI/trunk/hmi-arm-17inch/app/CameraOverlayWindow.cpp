#include "CameraOverlayWindow.h"

CameraOverlayWindow::CameraOverlayWindow(QWindow *parent) : QWindow(parent)
    , m_update_pending(false) {
    m_backingStore = new QBackingStore(this);
}

bool CameraOverlayWindow::event(QEvent *event) {

    // intercept all events for window
    // and render ( re-render ) if update window requested
    if (event->type() == QEvent::UpdateRequest) {
        m_update_pending = false;
        renderNow();
        return true;
    }
    return QWindow::event(event);
}

void CameraOverlayWindow::resizeEvent(QResizeEvent *resizeEvent) {
    // Resize backing store to necessary dimension if main window was resized
    m_backingStore->resize(resizeEvent->size());
    // rerender with new dimension if windows is visitlbe
    if (isExposed())
        renderNow();
}

void CameraOverlayWindow::exposeEvent(QExposeEvent *) {
    // rerender becasue window made visible
    if (isExposed()) {
        renderNow();
    }
}

void CameraOverlayWindow::renderNow() {

    if (!isExposed())
        return;

    // set size of the drawing area to the size of the window
    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);

    // Drawing background
    painter.fillRect(0, 0, width(), height(), Qt::transparent);

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}
