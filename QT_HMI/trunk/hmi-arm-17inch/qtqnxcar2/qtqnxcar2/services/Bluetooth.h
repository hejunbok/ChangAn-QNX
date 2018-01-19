#ifndef QTQNXCAR2_BLUETOOTH_H
#define QTQNXCAR2_BLUETOOTH_H

#include <QObject>

#include "qtqnxcar2_export.h"

class QAbstractItemModel;

namespace QPps {
class Object;
class Variant;
}

namespace QnxCar {
    class BluetoothDevicesModel;

/**
 * Bluetooth configuration and connection handling.
 *
 * This class is the central class for all handling of Bluetooth devices.
 * It allows scanning for devices, pairing, connecting and disconnecting.
 *
 * It wraps the following PPS objects:
 *
 * - @c /pps/services/bluetooth/control
 * - @c /pps/services/bluetooth/settings
 * - @c /pps/services/bluetooth/services
 * - @c /pps/services/bluetooth/status
 *
 * The models #remoteDevices and #pairedDevices list the contents of:
 *
 * - @c /pps/services/bluetooth/paired_devices/
 * - @c /pps/services/bluetooth/remote_devices/
 */
class QTQNXCAR2_EXPORT Bluetooth : public QObject
{
    Q_OBJECT

    /**
     * The accessibility level of the local bluetooth device
     *
     * @accessors accessibilityLevel(), setAccessibilityLevel()
     */
    Q_PROPERTY(AccessibilityLevel accessibilityLevel READ accessibilityLevel WRITE setAccessibilityLevel NOTIFY accessibilityLevelChanged)

    /**
     * The switch to turn the local bluetooth device on/off
     *
     * @accessors active(), setActive()
     */
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    /**
     * The friendly name of the local bluetooth device
     *
     * @accessors deviceName(), setDeviceName()
     */
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)

    /**
     * The address of the local bluetooth device
     *
     * @accessors deviceAddress()
     */
    Q_PROPERTY(QString deviceAddress READ deviceAddress NOTIFY deviceAddressChanged)

    /**
     * Whether an active connection to an remote bluetooth device exists
     *
     * @accessors activeConnectionAvailable()
     */
    Q_PROPERTY(bool activeConnectionAvailable READ activeConnectionAvailable NOTIFY activeConnectionAvailableChanged)

    /**
     * Whether a device search is currently running
     *
     * @accessors deviceSearchRunning()
     */
    Q_PROPERTY(bool deviceSearchRunning READ deviceSearchRunning NOTIFY deviceSearchRunningChanged)

    /**
     * Whether a pairing process is currently running
     *
     * @accessors pairingRunning()
     */
    Q_PROPERTY(bool pairingRunning READ pairingRunning NOTIFY pairingRunningChanged)

    /**
     * The address of the device for which pairing is running
     *
     * @accessors pairingDeviceAddress()
     */
    Q_PROPERTY(QString pairingDeviceAddress READ pairingDeviceAddress NOTIFY pairingDeviceAddressChanged)

    /**
     * Whether a connection to a hands-free profile is available
     *
     * @accessors handsFreeProfileAvailable()
     */
    Q_PROPERTY(bool handsFreeProfileAvailable READ handsFreeProfileAvailable NOTIFY handsFreeProfileAvailableChanged)

    /**
     * Whether a connection to a message access profile is available
     *
     * @accessors messageAccessProfileAvailable()
     */
    Q_PROPERTY(bool messageAccessProfileAvailable READ messageAccessProfileAvailable NOTIFY messageAccessProfileAvailableChanged)

    /**
     * Whether a connection to a phone book access profile is available
     *
     * @accessors phoneBookAccessProfileAvailable()
     */
    Q_PROPERTY(bool phoneBookAccessProfileAvailable READ phoneBookAccessProfileAvailable NOTIFY phoneBookAccessProfileAvailableChanged)

    /**
     * Whether a connection to a serial port profile is available
     *
     * @accessors serialPortProfileAvailable()
     */
    Q_PROPERTY(bool serialPortProfileAvailable READ serialPortProfileAvailable NOTIFY serialPortProfileAvailableChanged)

    /**
     * Whether a connection to a AVRCP is available
     *
     * @accessors audioProfileAvailable()
     */
    Q_PROPERTY(bool audioProfileAvailable READ audioProfileAvailable NOTIFY audioProfileAvailableChanged)

    /**
     * Whether a connection to a personal are network profile is available
     *
     * @accessors personalNetworkProfileAvailable()
     */
    Q_PROPERTY(bool personalNetworkProfileAvailable READ personalNetworkProfileAvailable NOTIFY personalNetworkProfileAvailableChanged)

    /**
     * The model that contains all found remote devices
     *
     * @accessors remoteDevices()
     */
    Q_PROPERTY(QnxCar::BluetoothDevicesModel* remoteDevices READ remoteDevices CONSTANT)

    /**
     * The model that contains all paired remote devices
     *
     * @accessors pairedDevices()
     */
    Q_PROPERTY(QnxCar::BluetoothDevicesModel* pairedDevices READ pairedDevices CONSTANT)

    /**
     * The result of the last pairing operation
     *
     * @accessors lastPairingResult()
     */
    Q_PROPERTY(PairingResult lastPairingResult READ lastPairingResult NOTIFY lastPairingResultChanged)

    /**
     * The state of the current connection
     *
     * @accessors connectionState()
     */
    Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)

    /**
     * The device's address of the current connection
     *
     * @accessors connectionDeviceAddress()
     */
    Q_PROPERTY(QString connectionDeviceAddress READ connectionDeviceAddress NOTIFY connectionDeviceAddressChanged)

    Q_ENUMS(AccessibilityLevel)
    Q_ENUMS(Service)
    Q_ENUMS(PairingResult)
    Q_ENUMS(ConnectionState)

public:
    /// Describes the accessibility level of the Bluetooth device
    enum AccessibilityLevel
    {
        NotAccessible,     ///< No discoverability and no connectability
        GeneralAccessible, ///< General discoverability and connectability
        LimitedAccessible, ///< Limited discoverability and connectability
        ConnectableOnly,   ///< Connectable but not discoverable
        DiscoverableOnly   ///< Discoverable but not connectable
    };

    /// Describes the profiles accessible through Bluetooth
    enum Service
    {
        HandsFreeService,       ///< Service to handle phone calls
        MessageAccessService,   ///< Service to access mails and SMS
        PhoneBookAccessService, ///< Service to access contacts
        PersonaAreaNetworkService, ///< Service to access network capabilities
        AVRCPService ///< Service to Audio capabilities
    };

    ///Describes the result of a pairing attempt
    enum PairingResult {
        PairingUnknown,
        PairingSuccessful, ///< The pairing was successful
        PairingFailed, ///< The pairing failed
        PairingCanceled ///<The pairing was canceled by the user
    };

    /// Describes the state of the connection to a paired device
    enum ConnectionState {
        Disconnected,      ///< Not connected
        Connecting,        ///< Connecting to the device
        Connected,         ///< Connection established
        Disconnecting,     ///< Disconnecting from the device
        ConnectFailed,     ///< Connection attempt failed
        DisconnectFailed,   ///< Disconnection attempt failed
        Undefined           ///< Undefined state, set on service startup
    };

    /**
     * Creates a new Bluetooth object.
     *
     * @param parent The parent object.
     */
    explicit Bluetooth(QObject *parent = 0);

    /// #accessibilityLevel
    AccessibilityLevel accessibilityLevel() const;
    /// #accessibilityLevel
    void setAccessibilityLevel(AccessibilityLevel level);

    /// #active
    bool active() const;
    /// #active
    void setActive(bool active);

    /// #deviceName
    QString deviceName() const;
    /// #deviceName
    void setDeviceName(const QString &name);

    /// #deviceAddress
    QString deviceAddress() const;

    /// #activeConnectionAvailable
    bool activeConnectionAvailable() const;
    /// #deviceSearchRunning
    bool deviceSearchRunning() const;
    /// #pairingRunning
    bool pairingRunning() const;
    /// #pairingDeviceAddress
    QString pairingDeviceAddress() const;
    /// #handsFreeProfileAvailable
    bool handsFreeProfileAvailable() const;
    /// #messageAccessProfileAvailable
    bool messageAccessProfileAvailable() const;
    /// #phoneBookAccessProfileAvailable
    bool phoneBookAccessProfileAvailable() const;
    /// #serialPortProfileAvailable
    bool serialPortProfileAvailable() const;
    /// #personalNetworkProfileAvailable
    bool personalNetworkProfileAvailable() const;
    /// #audioProfileAvailable
    bool audioProfileAvailable() const;

    /// #lastPairingResult
    PairingResult lastPairingResult() const;
    /// #connectionState
    ConnectionState connectionState() const;
    /// #connectionDeviceAddress
    QString connectionDeviceAddress() const;

    /// #remoteDevices
    BluetoothDevicesModel* remoteDevices() const;
    /// #pairedDevices
    BluetoothDevicesModel* pairedDevices() const;

public Q_SLOTS:
    /**
     * Initiates a new search for remote devices.
     *
     * Use the 'deviceSearchRunning' property to monitor the progress
     * of the search. The found devices are listed in the model provided
     * by the 'remoteDevices' property.
     */
    void initiateDeviceSearch();

    /**
     * Cancels a running search for remote devices.
     */
    void cancelDeviceSearch();

    /**
     * Initiates a pairing with the device of the given @p address.
     *
     * The address can be retrieved from the entries of the 'remoteDevices' model.
     * Use the 'pairingRunning' property to monitor the progress of the pairing
     * process.
     */
    void initiatePairing(const QString &address);

    /**
     * Cancels an ongoing pairing process with the device of the given @p address.
     */
    void cancelPairing(const QString &address);

    /**
     * Confirms or declines the pairing request for the device with the given @p address.
     *
     * This method should be called in response to the pairingConfirmationRequested() signal.
     */
    void confirmPairing(const QString &address, bool confirm);

    /**
     * Connects to the @p service at the device with the given @p address.
     */
    void connectService(const QString &address, Service service);

    /**
     * Connects to all services at the device with the given @p address.
     */
    void connectAllServices(const QString &address);

    /**
     * Connects to the @p service at the device with the given @p address.
     *
     * An overload of the above method, which takes a raw service address descriptor.
     * This is needed to connect to the serial port profile (SPP) as it must
     * contain the UUID of the service as well.
     */
    void connectService(const QString &address, const QString &service);

    /**
     * Disconnects from the @p service at the device with the given @p address.
     */
    void disconnectService(const QString &address, Service service);

    /**
     * Disconnects from the @p service at the device with the given @p address.
     *
     * An overload of the above method, which takes a raw service address descriptor.
     * This is needed to disconnect from the serial port profile (SPP) as it must
     * contain the UUID of the service as well.
     */
    void disconnectService(const QString &address, const QString &service);

    /**
     * Disconnects from all services at the device with the given @p address.
     */
    void disconnectAllServices(const QString &address);

    /**
     * Confirms or declines a connection request from the device with the given @p address.
     *
     * This method should be called in response to the connectionConfirmationRequested() signal.
     */
    void confirmConnection(const QString &address, bool confirm);

    /**
     * Removes a paired device, i.e. unpairs it and deletes the local settings regarding the device
     */
    void removeDevice(const QString &address);

    void disconnectBT(void);

Q_SIGNALS:
    // The change notification signals of the properties
    void accessibilityLevelChanged();
    void activeChanged();
    void deviceNameChanged();
    void deviceAddressChanged();
    void activeConnectionAvailableChanged(bool available);
    void deviceSearchRunningChanged();
    void pairingRunningChanged();
    void pairingDeviceAddressChanged();
    void handsFreeProfileAvailableChanged();
    void messageAccessProfileAvailableChanged();
    void phoneBookAccessProfileAvailableChanged();
    void serialPortProfileAvailableChanged();
    void audioProfileAvailableChanged();
    void personalNetworkProfileAvailableChanged();
    void lastPairingResultChanged();
    void connectionStateChanged();
    void connectionDeviceAddressChanged();

    /**
     * The device was deleted from list of paired devices (i.e. unpaired)
     *
     * @param address address of the deleted device
     */
    void deviceDeleted(const QString &address);

    /**
     * The device couldn't be deleted from list of paired devices (i.e. unpaired)
     *
     * @param address address of the device that could not be deleted
     */
    void deviceDeletionFailed(const QString &address);

    /**
     * Emitted when a pairing operation finished
     *
     * @param address the address of the peer device being paired
     * @param result the result of the pairing operation
     */
    void pairingFinished(const QString &address, QnxCar::Bluetooth::PairingResult result);

    /**
     * This signal is emitted whenever a pairing process must be confirmed.
     *
     * You should ask the user inside the connected slot whether he wants to
     * accept or decline the pairing and then call confirmPairing().
     *
     * @param address The address of the peer devices that should be paired.
     * @param pin The pin that authenticates the pairing process.
     */
    void pairingConfirmationRequested(const QString &address, const QString &pin);

    /**
     * This signal is emitted whenever a remote device wants to connect to a
     * service of the local device.
     *
     * You should ask the user inside the connected slot whether he wants to
     * accept or decline the connection and then call confirmConnection().
     *
     * @param address The address of the peer devices that wants to connect.
     */
    void connectionConfirmationRequested(const QString &address);

private:
    class Private;
    Private *const d;
};

}

#endif
