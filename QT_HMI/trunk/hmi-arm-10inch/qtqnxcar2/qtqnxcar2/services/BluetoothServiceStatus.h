#ifndef QTQNXCAR2_BLUETOOTHSERVICESTATUS_H
#define QTQNXCAR2_BLUETOOTHSERVICESTATUS_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {

/**
 * Class to access the connection and synchronization status
 * of bluetooth services.
 *
 * Wrapper class for messages and phonebook status PPS objects:
 *
 * - @c /pps/services/phonebook/status (when type Addressbook)
 * - @c /pps/services/messages/status (when type Messages)
 */
class QTQNXCAR2_EXPORT BluetoothServiceStatus : public QObject
{
    Q_OBJECT

    /**
     * The current connection state of the service
     *
     * @accessors state()
     */
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    /**
     * The current operational status of the service
     *
     * @accessors status()
     */
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_ENUMS(State)
    Q_ENUMS(Status)

public:
    /// Service connection states
    enum State {
        /// Unknown/base state
        Uninitialized,
        /// Connection to service established
        Connected,
        /// Connecting to service
        Connecting,
        /// Connection to service closed
        Disconnected
    };

    /// Service operational status
    enum Status {
        /// The service is not connected
        ErrorNotConnected,
        /// The service data synchronization is complete
        Complete,
        /// The service is busy
        ErrorBusy,
        /// The issued command was not known
        ErrorCommandNotKnown,
        /// Synchronization failed
        Failed,
        /// The service data is being synchronized
        Processing
    };

    /// Type of service
    enum ServiceType {
        /// Addressbook service
        Addressbook,
        /// Messages service (SMS, email)
        Messages
    };

    /**
     * Creates a wrapper for the given service @p type and connects to it
     * @param type the type of service to connect to
     * @param parent the QObject parent
     */
    explicit BluetoothServiceStatus(ServiceType type, QObject *parent = 0);

    /// #state
    State state() const;

    /// #status
    Status status() const;

Q_SIGNALS:
    void stateChanged(QnxCar::BluetoothServiceStatus::State state);
    void statusChanged(QnxCar::BluetoothServiceStatus::Status status);

private:
    class Private;
    Private* const d;
};

}

#endif
