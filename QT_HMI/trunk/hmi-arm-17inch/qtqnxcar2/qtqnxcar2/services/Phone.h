#ifndef QTQNXCAR2_PHONE_H
#define QTQNXCAR2_PHONE_H

#include <QObject>
#include <QStringList>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Gives access to the phone functionality of a mobile phone connected via Bluetooth
 *
 * Allows to manage incoming and outgoing calls (handsfree profile).
 *
 * Wrapper class for phone related PPS objects:
 * - @c /pps/services/handsfree/control
 * - @c /pps/services/handsfree/status
 */
class QTQNXCAR2_EXPORT Phone : public QObject
{
    Q_OBJECT

    /**
     * The current state of the phone.
     *
     * @accessors state()
     */
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    /**
     *  The current call's duration in milliseconds
     *
     * @accessors callDuration()
     */
    Q_PROPERTY(int callDuration READ callDuration NOTIFY callDurationChanged)

    Q_ENUMS(State)

public:
    /// Describes the state the phone can be in
    enum State
    {
        ErrorState,                     ///< An error occurred.
        InitializingState,              ///< Profile initializing.
        InitializedState,               ///< Profile initialized.
        ConnectedIdleState,             ///< No call activity; service is connected and idle.

        CallIncomingState,              ///< An incoming call.
        CallActiveState,                ///< An active call.
        CallOutgoingAlertingState,      ///< An outgoing call (alerting remote party).
        CallOutgoingDialingState,       ///< An outgoing call (dialing remote party's number).

        CallActiveHeldState,            ///< An active call and also one or more held calls.
        CallActiveHeldWaitingState,     ///< An active call, one or more held calls, as well as an incoming waiting call.
        CallActiveWaitingState,         ///< An active call and also an incoming waiting call.

        CallHeldState,                  ///< One or more held calls.
        CallHeldWaitingState,           ///< One or more held calls and also an incoming waiting call.
        CallHeldOutgoingAlertingState,  ///< One or more held calls and also an outgoing call (alerting remote party).
        CallHeldOutgoingDialingState,   ///< One or more held calls and also an outgoing call (dialing remote party's number).
        CallWaitingState                ///< A waiting call (used only for call list state info).
    };

    /**
     * Creates a new Phone object.
     *
     * @param parent The parent object.
     */
    explicit Phone(QObject *parent = 0);

    /// #state
    State state() const;

    /**
     * Returns the active phone number.
     *
     * @param service The phone service to request the active calls from.
     */
    Q_INVOKABLE QString activeCallId(const QString &service = QString()) const;

    /**
     * Returns the service identifier for handsfree service.
     */
    static Q_INVOKABLE QString handsFreeServiceIdentifier();

    /// #callDuration
    int callDuration() const;

public Q_SLOTS:
    /**
     * Accepts the incoming call.
     *
     * @param callerId The identifier of the call.
     * @param service The identifier of the phone service.
     */
    void accept(const QString &callerId, const QString &service = QString());

    /**
     * Dials a number.
     *
     * @param number The phone number to dial.
     * @param service The phone service to use for dialing.
     */
    void dial(const QString &number, const QString &service = QString());

    /**
     * Hangs up current active call.
     *
     * @param callerId The identifier of the call.
     * @param service The identifier of the phone service.
     */
    void hangUp(const QString &callerId, const QString &service = QString());

    /**
     * Puts a call on hold.
     *
     * @param callerId The identifier of the call.
     * @param hold @c true to put the current call on hold, @c false to release current call from hold.
     * @param service The identifier of the phone service.
     */
    void hold(const QString &callerId, const QString &service = QString());

    /**
     * Mutes audio input for incoming phone call.
     *
     * @param callerId The identifier of the call.
     * @param hold @c true to mute, @c false to unmute.
     * @param service The identifier of the phone service.
     */
    void mute(const QString &callerId, const QString &service = QString());

    /**
     * Redials the last called number.
     *
     * @param service The identifier of the phone service.
     */
    void redial(const QString &service = QString());

    /**
     *  Updates the current call duration.
     *  triggered by the call duration timer
     */
    void updateCallDuration();

Q_SIGNALS:
    void stateChanged(QnxCar::Phone::State state);

    /**
     * This signal is emitted whenever there is an incoming call.
     *
     * Call accept(), hold() or hangup() in the slot connected to this signal.
     *
     * @param callId The identifier for the incoming call.
     * @param service The identifier for the service that handles the call.
     */
    void incomingCall(const QString &callId, const QString &service);
    /**
     * This signal is emitted whenever there is an outgoing call.
     *
     * @param callId The identifier for the outgoing call.
     * @param service The identifier for the service that handles the call.
     */
    void outgoingCall(const QString &callId, const QString &service);
    /**
     * @brief outgoingCallAlerting is emitted whenever the call has reached the outgoing alerting state
     *
     * @param callId The identifier for the outgoing call.
     * @param service The identifier for the service that handles the call.
     */
    void outgoingCallAlerting(const QString &callId, const QString &service);
    /**
     * This signal is emitted whenever a call becomes active (e.g. recipient
     * accepted the outgoing call or incoming call is accepted locally).
     *
     * @param callId The identifier for the active call.
     * @param service The identifier for the service that handles the call.
     */
    void callActivated(const QString &callId, const QString &service);

    /**
     * This signal is emitted whenever the phone switches to idle state
     * (no incoming, outgoing or active calls) and is ready to accept
     * commands.
     *
     * @param service The identifier for the service that is ready now.
     */
    void phoneReady(const QString &service);

    /**
      * This signal is emitted whenever the current call duration time has changed.
      */
    void callDurationChanged();

private:
    class Private;
    Private* const d;
};

}

#endif
