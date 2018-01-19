#ifndef QTQNXCAR2_SENSORS_P_H
#define QTQNXCAR2_SENSORS_P_H

#include "Sensors.h"

#include <qpps/object.h>

namespace QnxCar {

class Sensors::Private : public QObject
{
    Q_OBJECT

public:
    Private(Sensors *qq);

    Sensors *q;

    QPps::Object *m_ppsObject;
    bool m_brakeAbs;
    int m_laneDeparture;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
