#include "BluetoothDevicesModel.h"
#include "BluetoothDevicesModel_p.h"

#include <qpps/changeset.h>

#include <QDir>

namespace QnxCar {

static const QString PPS_BLUETOOTH_BASE = "/pps/services/bluetooth/";

static QString toDirectory(BluetoothDevicesModel::Type type)
{
    switch (type) {
    case BluetoothDevicesModel::PairedDevices:
        return PPS_BLUETOOTH_BASE + "paired_devices/";
    case BluetoothDevicesModel::RemoteDevices:
        return PPS_BLUETOOTH_BASE + "remote_devices/";
    default:
        Q_ASSERT(!"Invalid enum value for BluetoothDevicesModel::Type");
        return QString();
    }
}

BluetoothDevice::BluetoothDevice()
    : paired(false)
    , inRange(false)
{
}

BluetoothDevicesModel::Private::Private(BluetoothDevicesModel *qq, Type type)
    : QObject(qq)
    , q(qq)
    , m_devicesDir(toDirectory(type))
    , m_dirWatcher(new QPps::DirWatcher(m_devicesDir))
{
    connect(m_dirWatcher, SIGNAL(objectAdded(QString)), this, SLOT(reloadDeviceList()));
    connect(m_dirWatcher, SIGNAL(objectRemoved(QString)), this, SLOT(reloadDeviceList()));
}

void BluetoothDevicesModel::Private::reloadDeviceList()
{
    const QStringList objects = m_dirWatcher->objectNames();

    //delete removed objects
    QMap<QString, QPps::Object*>::Iterator it = m_ppsDevices.begin();
    while (it != m_ppsDevices.end()) {
        if (!objects.contains(it.key())) {
            removeDevice(it.key());
            delete it.value();
            it = m_ppsDevices.erase(it);
        } else {
            ++it;
        }
    }

    Q_FOREACH(const QString &object, objects)
        addDeviceFromName(object);
}

void BluetoothDevicesModel::Private::objectChanged(const QPps::Changeset &changes)
{
    Q_UNUSED(changes)
    QPps::Object *object = qobject_cast<QPps::Object*>(sender());
    Q_ASSERT(object);
    addDevice(object);
}

void BluetoothDevicesModel::Private::addDevice(const QPps::Object *object)
{
    //Skip dubious file ..sss containing "super::hack"
    if (object->attribute(QStringLiteral("super")) == QStringLiteral("hack"))
        return;
    BluetoothDevice device;
    device.address = QFileInfo(object->path()).fileName();
    device.name = object->attribute(QStringLiteral("name")).toString();
    device.paired = object->attribute(QStringLiteral("paired")).toBool();
    const QPps::Variant inRange = object->attribute(QStringLiteral("in_range"));
    if (inRange.isValid())
        device.inRange = inRange.toBool();
    //TODO parse services etc.
    addDevice(device);
}

void BluetoothDevicesModel::Private::addDeviceFromName(const QString &name)
{
    if (m_ppsDevices.contains(name))
        return;
    QPps::Object *object = new QPps::Object(m_devicesDir + name, QPps::Object::SubscribeMode, true, q);
    object->setAttributeCacheEnabled(true);
    connect(object, SIGNAL(attributesChanged(QPps::Changeset)), this, SLOT(objectChanged(QPps::Changeset)));
    m_ppsDevices.insert(name, object);
    addDevice(object);
}

void BluetoothDevicesModel::Private::addDevice(const BluetoothDevice &device)
{
    for (int row = 0; row < m_devices.count(); ++row) {
        const BluetoothDevice &bluetoothDevice = m_devices[row];
        if (device.address == bluetoothDevice.address) {
            // Device is known already, so just update
            m_devices[row] = device;

            const QModelIndex index = q->index(row, 0);
            emit q->dataChanged(index, index);
            return;
        }
    }

    // Device is new, so append it
    q->beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
    m_devices.append(device);
    q->endInsertRows();
}

void BluetoothDevicesModel::Private::removeDevice(const QString &deviceAddress)
{
    for (int row = 0; row < m_devices.count(); ++row) {
        const BluetoothDevice &bluetoothDevice = m_devices[row];
        if (deviceAddress == bluetoothDevice.address) {
            q->beginRemoveRows(QModelIndex(), row, row);
            m_devices.remove(row);
            q->endRemoveRows();
            return;
        }
    }
}

BluetoothDevice BluetoothDevicesModel::Private::findDeviceByAddress(const QString &address) const
{
    Q_FOREACH(const BluetoothDevice &device, m_devices) {
        if (device.address == address)
            return device;
    }
    return BluetoothDevice();
}

BluetoothDevicesModel::BluetoothDevicesModel(Type type, QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this, type))
{
    d->reloadDeviceList();
}

int BluetoothDevicesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->m_devices.count();
}

int BluetoothDevicesModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant BluetoothDevicesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const BluetoothDevice &device = d->m_devices[index.row()];

    switch (role) {
    case AddressRole:
        return device.address;
    case Qt::DisplayRole:
    case NameRole:
        return device.name;
    case PairedRole:
        return device.paired;
    case InRangeRole:
        return device.inRange;
    case ClassRole:
        return device.classCode;
    case ServicesRole:
        return device.services;
    }

    return QVariant();
}

QHash<int, QByteArray> BluetoothDevicesModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();

    names.insert(AddressRole, "address");
    names.insert(NameRole, "name");
    names.insert(PairedRole, "paired");
    names.insert(InRangeRole, "inRange");
    names.insert(ClassRole, "class");
    names.insert(ServicesRole, "services");

    return names;
}

int BluetoothDevicesModel::rowForDeviceId(const QString &deviceId) const
{
    const QModelIndexList results = match(index(0, 0), AddressRole, deviceId, 1, Qt::MatchExactly);
    if (results.isEmpty())
        return -1;
    return results.first().row();
}

QString BluetoothDevicesModel::nameForAddress(const QString &address) const
{
    const BluetoothDevice device = d->findDeviceByAddress(address);
    return device.name;
}

}
