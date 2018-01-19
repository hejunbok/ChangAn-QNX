#ifndef QTQNXCAR2_BLUETOOTHDEVICESMODEL_P_H
#define QTQNXCAR2_BLUETOOTHDEVICESMODEL_P_H

#include "BluetoothDevicesModel.h"

#include <QObject>
#include <QStringList>
#include <QVector>

#include <qpps/dirwatcher.h>
#include <qpps/object.h>

namespace QnxCar {

struct BluetoothDevice
{
    BluetoothDevice();
    QString address;
    QString name;
    bool paired;
    bool inRange;
    QString classCode;
    QStringList services;
};

class BluetoothDevicesModel::Private : public QObject
{
    Q_OBJECT
public:
    Private(BluetoothDevicesModel *qq, Type type);

    void addDevice(const QPps::Object *object);
    void addDevice(const BluetoothDevice &device);
    void removeDevice(const QString &deviceAddress);
    void addDeviceFromName(const QString &name);

    BluetoothDevice findDeviceByAddress(const QString &address) const;

public Q_SLOTS:
    void reloadDeviceList();
    void objectChanged(const QPps::Changeset &changes);

public:
    BluetoothDevicesModel* q;

    QString m_devicesDir;
    QVector<BluetoothDevice> m_devices;
    QPps::DirWatcher *m_dirWatcher;
    QMap<QString,QPps::Object*> m_ppsDevices;
};

}

#endif
