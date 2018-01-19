#ifndef QTQNXCAR2_BLUETOOTHDEVICESMODEL_H
#define QTQNXCAR2_BLUETOOTHDEVICESMODEL_H

#include <QAbstractListModel>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Model listing Bluetooth devices.
 *
 * This model lists bluetooth devices, either all scan results (RemoteDevices),
 * or paired devices only (PairedDevices).
 *
 * @sa Bluetooth::remoteDevices, Bluetooth::pairedDevices
 */
class QTQNXCAR2_EXPORT BluetoothDevicesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /// Item roles
    enum Roles {
        AddressRole = Qt::UserRole + 1, ///< The MAC address of the bluetooth device, role name @c "address"
        NameRole,                       ///< The friendly name of the bluetooth device, role name @c "name"
        PairedRole,                     ///< Indicates whether the device is paired, role name @c "paired"
        InRangeRole,                    ///< Indicates whether the device is in range, role name @c "inRange"
        ClassRole,                      ///< The class code of the device, role name @c "class"
        ServicesRole                    ///< The list of service profiles the device offers, role name @c "services"
    };

    /// Model content type filter
    enum Type {
        /// Populate model with paired devices
        PairedDevices,
        /// Populate model with devices found by scanning, paired and unpaired
        RemoteDevices
    };

    /**
     * Creates the instance and populates it with devices of the given @p type
     *
     * @param type the type of devices to expose through this model
     * @param parent the QObject parent
     */
    explicit BluetoothDevicesModel(Type type, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /**
     * Returns the row in the model for a given device
     *
     * @param deviceId the identifier to match devices with
     * @return the row the given device is on or @c -1 if there is no match
     * @sa #AddressRole
     */
    Q_INVOKABLE int rowForDeviceId(const QString &deviceId) const;

    /**
     * Returns the name of a device with a given @p address
     *
     * @param address the device address to look for
     * @return the name of the device or QString() if none of the model's devices has @p address
     * @sa #AddressRole, #NameRole
     */
    Q_INVOKABLE QString nameForAddress(const QString &address) const;

private:
    friend class Bluetooth;

    class Private;
    Private* const d;
};

}

#endif
