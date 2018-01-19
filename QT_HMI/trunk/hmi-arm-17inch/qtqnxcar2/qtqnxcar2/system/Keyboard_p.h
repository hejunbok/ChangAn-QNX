#ifndef QTQNXCAR2_KEYBOARD_P_H
#define QTQNXCAR2_KEYBOARD_P_H

#include "Keyboard.h"

#include <qpps/object.h>

#include <QStringList>

namespace QnxCar {

class Keyboard::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(Keyboard *qq);

    Keyboard * const q;
    QPps::Object *ppsControl;
    QPps::Object *ppsStatus;
    QStringList layoutsAvailable;

private Q_SLOTS:
    void controlObjectChanged();
    void statusAttributeChanged(const QString &name, const QPps::Variant &variant);
};

}

#endif
