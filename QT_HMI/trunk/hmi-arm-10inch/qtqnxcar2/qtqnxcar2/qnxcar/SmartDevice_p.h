#ifndef QTQNXCAR2_SMART_DEVICE_P_H
#define QTQNXCAR2_SMART_DEVICE_P_H

#include "SmartDevice.h"

#include <qpps/object.h>

namespace QnxCar {

class SmartDevice::Private : public QObject
{
    Q_OBJECT

public:
    Private(SmartDevice *qq);

    SmartDevice *q;

    QPps::Object *m_ppsObject;

    int m_heartRate;
    int m_pulse;
    int m_sleepQuality;
    int m_runningSteps;


private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
