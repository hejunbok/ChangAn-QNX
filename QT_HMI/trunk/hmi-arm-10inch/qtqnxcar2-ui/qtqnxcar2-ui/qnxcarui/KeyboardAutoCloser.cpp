#include "KeyboardAutoCloser.h"

#include <QDebug>

#include <QGuiApplication>
#include <QMouseEvent>
#include <QQuickItem>
#include <QTouchEvent>

//#define KBDAUTOCLOSER_DEBUG

using namespace QnxCarUi;

KeyboardAutoCloser::KeyboardAutoCloser(QObject *parent) :
    QObject(parent),
    m_inputMethodVisible(false),
    m_focusItem(0)
{
    QCoreApplication::instance()->installEventFilter(this);
    connect(QGuiApplication::inputMethod(), SIGNAL(visibleChanged()),
            this, SLOT(inputMethodVisibleChanged()));
}

void KeyboardAutoCloser::inputMethodVisibleChanged()
{
    m_inputMethodVisible = QGuiApplication::inputMethod()->isVisible();
    m_focusItem = 0;
    if (!m_inputMethodVisible)
        return;

    QObject *focusObject = QGuiApplication::focusObject();
    if (QQuickItem *item = qobject_cast<QQuickItem *>(focusObject)) {
        m_focusItem = item;
    } else {
        qWarning() << Q_FUNC_INFO << "no input widget seems to have focus?!";
    }
}

QRectF KeyboardAutoCloser::focusItemRect() const
{
    if (m_focusItem) {
        const QRectF itemRect(0.0, 0.0, m_focusItem->width(), m_focusItem->height());
        return m_focusItem->mapRectToScene(itemRect);
        // ### We should map from scene coordinates to screen coordinates, which is only not necessary
        // on the hardware because the application runs full screen. I have not found a method to perform
        // that mapping. Maybe we could use an event filter only on the scene to avoid the problem.
        // This is related to the other problem:
        // In the general case, we'd also have to make everything work with multi-window applications,
        // so that clicks in one window don't affect the other. This may or may not be taken care of
        // by items losing focus and the keyboard going away when switching windows.
    }
    return QRectF();
}

bool KeyboardAutoCloser::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    if (!m_inputMethodVisible)
        return false;

    QPointF inputPoint;
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        inputPoint = mouseEvent->pos();
    } else if (event->type() == QEvent::TouchBegin) {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

        // ### this might not work as intended with multitouch
        inputPoint = touchEvent->touchPoints().first().screenPos();
    } else {
        return false;
    }

    const QRectF inputItemRect = focusItemRect();
    if (!inputItemRect.contains(inputPoint)) {
        QGuiApplication::inputMethod()->hide();
    }

    return false;
}
