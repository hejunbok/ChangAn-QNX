#ifndef KEYBOARDAUTOCLOSER_H
#define KEYBOARDAUTOCLOSER_H

#include "qtqnxcar_ui_export.h"

#include <QObject>

class QQuickItem;
class QRectF;

namespace QnxCarUi {

/**
 * This class hides the virtual keyboard on click / touch events outside of current input widget.
 * If the current input widget loses focus as a result of that click, keyboard hiding is already
 * handled in the platform plugin. If the input widget does not lose focuse as a consequence of
 * the click, this class hides the keyboard. That is why it exists.
 */

class QTQNXCAR_UI_EXPORT KeyboardAutoCloser : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardAutoCloser(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

public slots:
    void inputMethodVisibleChanged();

private:
    QRectF focusItemRect() const;

    bool m_inputMethodVisible;
    QQuickItem *m_focusItem;
};

}

#endif
