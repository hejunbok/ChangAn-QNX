#ifndef QTQNXCAR2_BLUETOOTHSERVICESTATUS_P_H
#define QTQNXCAR2_BLUETOOTHSERVICESTATUS_P_H

#include "BluetoothServiceStatus.h"

#include <qpps/object.h>

namespace QnxCar {

class BluetoothServiceStatus::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(BluetoothServiceStatus::ServiceType type, BluetoothServiceStatus *q);
    virtual ~Private();

    BluetoothServiceStatus *const q;

    QPps::Object *m_ppsStatusObject;

    State m_state;
    Status m_status;

private Q_SLOTS:
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
