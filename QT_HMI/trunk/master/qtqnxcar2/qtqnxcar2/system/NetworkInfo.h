#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <QObject>

#include "pps/ControlCommand.h"
#include "qtqnxcar2_export.h"

namespace QPps {
class Variant;
}

namespace QnxCar {

/**
 * Network status information
 *
 * Monitors the configuration of the network interface.
 *
 * Wrapper around PPS objects:
 * - status: @c /pps/services/networking/all/status_public
 * - interface data: @c /pps/services/networking/all/interfaces/
 */
class QTQNXCAR2_EXPORT NetworkInfo : public QObject
{
    Q_OBJECT

    /**
     * IP address of the current default interface
     *
     * @accessors ipAddress(), setIpAddress()
     */
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)

    /**
     * Netmask of the current default interface
     *
     * @accessors netmask(), setNetmask()
     */
    Q_PROPERTY(QString netmask READ netmask WRITE setNetmask NOTIFY netmaskChanged)

    /**
     * Gateway IP address of the current default interface
     *
     * @accessors gateway(), setGateway()
     */
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)


    /**
     * Whether DHCP has been used for address assignment
     *
     * @accessors dhcp(), setDhcp()
     */
    Q_PROPERTY(bool dhcp READ dhcp WRITE setDhcp NOTIFY dhcpChanged)

public:
    /**
     * Connects to PPS and retrieves the current default interface's data
     * @param parent the QObject parent
     */
    explicit NetworkInfo(QObject *parent = 0);
    ~NetworkInfo();

    /**
     * Get current IP address
     * @return IP Address String
     */
    QString ipAddress() const;
    /**
     * Get current netmask
     * @return netmask string
     */
    QString netmask() const;
    /**
     * Get gateway for active dafault interface
     * @return  gateways string
     */
    QString gateway() const;
    /**
     * Indicated if this is DHCP obtaine settings or set manually
     * @return true - if DHCP, false if manual
     */
    bool dhcp() const;

public slots:
    /**
     * Invoked from HMI to set static network lconfiguration
     * @param ipAddress QString ip address string
     * @param netmask QString netmask string
     * @param gateway QString gateway string
     */
    void setStaticNetworkConfig(const QString &ipAddress,const QString &netmask,const QString &gateway);
    /**
     * Invoked from HMI to set dynamic network settings (DHCP)
     */
    void setDynamicNetworkConfig();

signals:
    /**
     * Signals when IP Address Changed
     * @param ipAddress QString ip address string
     */
    void ipAddressChanged(const QString &ipAddress);
    /**
     * Signals that netmasjk changed
     * @param netmask QString netmask string
     */
    void netmaskChanged(const QString &netmask);
    /**
     * Signals that gateways changed
     * @param gateway QString gateways string
     */
    void gatewayChanged(const QString &gateway);
    /**
     * Signals that DHCP settiong changed
     * @param dhcp true - DHCP enabled, false - DHCP disabled
     */
    void dhcpChanged(bool dhcp);
    /**
     * Signals that configuration saved
     */
    void configurationSaved();

private slots:
    void updateNetworkInfo(const QString &attribute, const QPps::Variant &value);
    void updateInterfaceInfo(const QString &attribute, const QPps::Variant &value);
    void onReply(const QString &attribute, const QPps::Variant &value);

private:
    void updateFromInterface(const QString &identifier);
    /// #ipAddress
    void setIpAddress(const QString &ipAddress);
    /// #netmask
    void setNetmask(const QString &netmask);
    /// #gateway
    void setGateway(const QString &gateway);
    /// #dhcp
    void setDhcp(bool dhcp);

    void connectNetwork(const QString &ipAddress,const QString &netmask,const QString &gateway);
    void disconnectNetwork();
private:
    class Private;
    Private *const d;
};

}

#endif // NETWORKINFO_H
