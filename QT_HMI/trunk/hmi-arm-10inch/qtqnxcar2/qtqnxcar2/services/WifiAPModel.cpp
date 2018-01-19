#include "WifiAPModel.h"

#include <qpps/changeset.h>
#include <QDebug>
#include <QDir>

namespace QnxCar {

    WifiAccessPoint::WifiAccessPoint()
        : connected(false)
    {
    }

    WifiAPModel::WifiAPModel(QObject *parent): QAbstractListModel(parent)
    {
    }

    int WifiAPModel::rowCount(const QModelIndex &parent) const {
        return parent.isValid() ? 0 : m_aps.count();
    }

    int WifiAPModel::columnCount(const QModelIndex &parent) const {
        return parent.isValid() ? 0 : 1;
    }

    QVariant WifiAPModel::data(const QModelIndex &index, int role) const {
        const WifiAccessPoint &device = m_aps[index.row()];

        switch (role) {
        case BSSIDRole:
            return device.bssid;
        case Qt::DisplayRole:
        case NameRole:
            return device.name;
        case SSIDRole:
            return device.ssid;
        case SecureRole:
            return device.secure;
        case ConnectedRole:
            return device.connected;
        }

        return QVariant();
    }

    void WifiAPModel::addDevice(const WifiAccessPoint &device)
    {
        for (int row = 0; row < m_aps.count(); ++row) {
            const WifiAccessPoint &wifiDevice = m_aps[row];
            if (device.bssid == wifiDevice.bssid) {
                // Device is known already, so just update
                m_aps[row] = device;

                const QModelIndex index = this->index(row, 0);
                emit dataChanged(index, index);

                return;
            }
        }

        // Device is new, so append it
        beginInsertRows(QModelIndex(), m_aps.count(), m_aps.count());
        m_aps.append(device);
        endInsertRows();
    }

    void WifiAPModel::removeDevice(const QString &bssid)
    {
        for (int row = 0; row < m_aps.count(); ++row) {
            const WifiAccessPoint &wifiDevice = m_aps[row];
            if (bssid == wifiDevice.bssid) {
                beginRemoveRows(QModelIndex(), row, row);
                m_aps.remove(row);
                endRemoveRows();
                return;
            }
        }
    }

    void WifiAPModel::removeAllDevices()
    {
        beginRemoveRows(QModelIndex(), 0, m_aps.count());
        m_aps.clear();
        endRemoveRows();
    }

    WifiAccessPoint WifiAPModel::findDeviceByAddress(const QString &bssid) const
    {
        Q_FOREACH(const WifiAccessPoint &device, m_aps) {
            if (device.bssid == bssid)
                return device;
        }
        return WifiAccessPoint();
    }

    QHash<int, QByteArray> WifiAPModel::roleNames() const
    {
        QHash<int, QByteArray> names = QAbstractListModel::roleNames();

        names.insert(NameRole, "name");
        names.insert(BSSIDRole, "bssid");
        names.insert(SSIDRole, "ssid");
        names.insert(SecureRole, "secure");
        names.insert(ConnectedRole, "connected");

        return names;
    }
}
