#ifndef WIFIAPMODEL_H
#define WIFIAPMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "qtqnxcar2_export.h"
namespace QnxCar {

struct WifiAccessPoint
{
    WifiAccessPoint();
    QString bssid;          // kind of mac address, unique
    QString ssid;           // AP name, not unique
    QString name;           // AP name, not unique
    QString frequency;
    QString signalLevel;
    QString identity;
    QString password;
    QString key_mgmt;

    QString ip_address;     // IP addres acquire
    QString address;        // MAC

    bool connected;
    bool secure;
};

/**
 * Model listing Wifi access points (AP).
 *
 * This model lists WIFI APs, provided by scan results
 */
class QTQNXCAR2_EXPORT WifiAPModel : public QAbstractListModel
{
    Q_OBJECT
public:

    /// Item roles
    enum Roles {
        BSSIDRole = Qt::UserRole + 1,
        NameRole,
        SSIDRole,
        SecureRole,
        ConnectedRole
    };

    explicit WifiAPModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    void addDevice(const WifiAccessPoint &ap);
    void removeDevice(const QString &bssid);
    void removeAllDevices();
    WifiAccessPoint findDeviceByAddress(const QString &bssid) const;

private:
    friend class Wifi;
    QVector<WifiAccessPoint> m_aps;
};
}

#endif // WIFIAPMODEL_H
