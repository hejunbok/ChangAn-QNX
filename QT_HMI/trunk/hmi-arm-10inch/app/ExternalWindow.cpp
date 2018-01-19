#include "ExternalWindow.h"

ExternalWindow::ExternalWindow(WindowManager *windowManager, QObject *parent)
    : QObject(parent)
    , m_windowManager(windowManager)
    , m_visible(false)
    , m_z(0)
    , m_sensitivity(WindowManager::Never)
    , m_pid()
    , m_shownOnScreen(false)
{
    Q_ASSERT(m_windowManager);
}

bool ExternalWindow::visible() const
{
    return m_visible;
}

void ExternalWindow::setVisible(bool visible)
{
    if (visible == m_visible)
        return;
    m_visible = visible;
    emit visibleChanged(m_visible);

#if 0 //Handled externally to sync with QQuickView's frame swapping (QCARTWO-5048)
    if (m_pid != Q_PID())
        m_windowManager->setVisible(m_pid, m_visible);
#endif
}

QRect ExternalWindow::geometry() const
{
    return m_geometry;
}

void ExternalWindow::setGeometry(const QRect &geometry)
{
    if (m_geometry == geometry)
        return;
    m_geometry = geometry;
    emit geometryChanged(m_geometry);
    emit xChanged(m_geometry.x());
    emit yChanged(m_geometry.y());
    emit widthChanged(m_geometry.width());
    emit heightChanged(m_geometry.height());

    m_windowManager->setPosition(m_pid, m_geometry.topLeft());
    m_windowManager->setSize(m_pid, m_geometry.size());
}

Q_PID ExternalWindow::pid() const
{
    return m_pid;
}

void ExternalWindow::setPid(Q_PID pid)
{
    if (m_pid == pid)
        return;

    m_pid = pid;

    m_windowManager->setSensitivity(m_pid, (WindowManager::Sensitivities)m_sensitivity);
    m_windowManager->setVisible(m_pid, m_visible);
    m_windowManager->setPosition(m_pid, m_geometry.topLeft());
    m_windowManager->setSize(m_pid, m_geometry.size());
    m_windowManager->setZValue(m_pid, m_z);

    m_windowManager->setSourceClipSize(m_pid,m_sourceClipSize);
    m_windowManager->setSourceSize(m_pid,m_sourceSize);
}

int ExternalWindow::x() const
{
    return m_geometry.x();
}

void ExternalWindow::setX(int x)
{
    if (x == m_geometry.x())
        return;
    m_geometry.setX(x);
    emit xChanged(m_geometry.x());

    if (m_pid != Q_PID())
        m_windowManager->setPosition(m_pid, m_geometry.topLeft());
}

int ExternalWindow::y() const
{
    return m_geometry.y();
}

void ExternalWindow::setY(int y)
{
    if (y == m_geometry.y())
        return;
    m_geometry.setY(y);
    emit yChanged(m_geometry.y());

    if (m_pid != Q_PID())
        m_windowManager->setPosition(m_pid, m_geometry.topLeft());
}

int ExternalWindow::width() const
{
    return m_geometry.width();
}

void ExternalWindow::setWidth(int width)
{
    if (width == m_geometry.width())
        return;
    m_geometry.setWidth(width);
    emit widthChanged(m_geometry.width());

    if (m_pid != Q_PID())
        m_windowManager->setSize(m_pid, m_geometry.size());
}

int ExternalWindow::height() const
{
    return m_geometry.height();
}

void ExternalWindow::setHeight(int height)
{
    if (height == m_geometry.height())
        return;
    m_geometry.setHeight(height);
    emit heightChanged(m_geometry.height());

    if (m_pid != Q_PID())
        m_windowManager->setSize(m_pid, m_geometry.size());
}

int ExternalWindow::z() const
{
    return m_z;
}

void ExternalWindow::setZ(int z)
{
    if (z == m_z)
        return;
    m_z = z;
    emit zChanged(m_z);

    if (m_pid != Q_PID())
        m_windowManager->setZValue(m_pid, m_z);
}

void ExternalWindow::setClipPosition(int x, int y)
{
    QPoint point(x,y);
    m_clipPosition = point;
    if (m_pid != Q_PID()) {
        m_windowManager->setClipPosition(m_pid, point);
    }
}

void ExternalWindow::setClipSize(int w, int h)
{
   QSize size(w,h);
   m_clippingSize = size;
   if (m_pid != Q_PID()) {
         m_windowManager->setClipSize(m_pid, size);
   }
}

void ExternalWindow::setSourceSize(int w, int h)
{
   QSize size(w,h);
   m_sourceSize = size;
   if (m_pid != Q_PID()) {
         m_windowManager->setSourceSize(m_pid, size);
   }
}

void ExternalWindow::setSourceClipSize(int w, int h)
{
   QSize size(w,h);
   m_sourceClipSize = size;
   if (m_pid != Q_PID()) {
         m_windowManager->setSourceClipSize(m_pid, size);
   }
}

void ExternalWindow::setSensitivity(const WindowManager::Sensitivities sensetivity) {
    m_sensitivity = sensetivity;
    if (m_pid != Q_PID()) {
          m_windowManager->setSensitivity(m_pid, (WindowManager::Sensitivities)sensetivity);
    }
}

bool ExternalWindow::shownOnScreen() const
{
    return m_shownOnScreen;
}

void ExternalWindow::handleApplicationStarted()
{
    if (m_pid == Q_PID())
        return;

    m_shownOnScreen = true;
    emit shownOnScreenChanged(true);
}
