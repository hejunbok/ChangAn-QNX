#include "Phone.h"
#include "Phone_p.h"

#include <qpps/changeset.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_stateKey = "state";
static const char* pps_stateParameterKey = "state_param";

static Phone::State stringToState(const QString &stringData)
{
    static QHash<QString, Phone::State> hash;
    if (hash.isEmpty()) {
        hash.insert("HFP_ERROR", Phone::ErrorState);
        hash.insert("HFP_INITIALIZING", Phone::InitializingState);
        hash.insert("HFP_INITIALIZED", Phone::InitializedState);
        hash.insert("HFP_CONNECTED_IDLE", Phone::ConnectedIdleState);
        hash.insert("HFP_CALL_INCOMING", Phone::CallIncomingState);
        hash.insert("HFP_CALL_ACTIVE", Phone::CallActiveState);
        hash.insert("HFP_CALL_OUTGOING_ALERTING", Phone::CallOutgoingAlertingState);
        hash.insert("HFP_CALL_OUTGOING_DIALING", Phone::CallOutgoingDialingState);
        hash.insert("HFP_CALL_ACTIVE_HELD", Phone::CallActiveHeldState);
        hash.insert("HFP_CALL_ACTIVE_HELD_WAITING", Phone::CallActiveHeldWaitingState);
        hash.insert("HFP_CALL_ACTIVE_WAITING", Phone::CallActiveWaitingState);
        hash.insert("HFP_CALL_HELD", Phone::CallHeldState);
        hash.insert("HFP_CALL_HELD_WAITING", Phone::CallHeldWaitingState);
        hash.insert("HFP_CALL_HELD_OUTGOING_ALERTING", Phone::CallHeldOutgoingAlertingState);
        hash.insert("HFP_CALL_HELD_OUTGOING_DIALING", Phone::CallHeldOutgoingDialingState);
        hash.insert("HFP_CALL_WAITING", Phone::CallWaitingState);
    }

    const QHash<QString, Phone::State>::const_iterator it = hash.constFind(stringData);
    if (it == hash.constEnd()) {
        qWarning() << "Phone::stringToState: unknown state:" << stringData;
        return Phone::ErrorState;
    }

    return *it;
}

Phone::Private::Private(Phone *qq)
    : QObject(qq)
    , q(qq)
    , m_state(Phone::InitializingState)
{
    m_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/handsfree/control"),
                                          QPps::Object::PublishMode, true, this);
    m_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/services/handsfree/status"),
                                          QPps::Object::SubscribeMode, true, this);

    if (m_ppsStatusObject->isValid()) {
        connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsStatusObject->setAttributeCacheEnabled(true);

        m_state = stringToState(m_ppsStatusObject->attribute(QString::fromLatin1(pps_stateKey)).toString());
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
    }

    if (!m_ppsControlObject->isValid())
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsControlObject->errorString();

    connect(&m_callDurationTimer, SIGNAL(timeout()), qq, SLOT(updateCallDuration()));
}

QString Phone::Private::currentCallId(const QString &param) const
{
    if (param.isEmpty() || param == QStringLiteral("NA"))
        return m_currentCallId;
    else
        return param;
}

void Phone::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{

    /*
        Note: there is a bit of a flaw in the design of this module, we are treating
        the change events from PPS as states, however we receive multiple events from
        PPS reflecting the same state. There is also no guarantee that the data in each event
        is the same. Originally the function was only running logic if the state had changed
        however that is not feasible, in the case of CallOutgoingDialingState it is not uncommon
        to receive multiple events and only on the third event(for illustration purposes) do we
        actually receive call data. To mitigate this there is as much filtering as possible per case
        to reduce re-running logic code however some signals will be emitted multiple times
    */
    if (name == QString::fromLatin1(pps_stateKey)) {

        const State state = stringToState(attribute.toString());
        const State oldState = m_state;
        const QString param = m_ppsStatusObject->attribute(QString::fromLatin1(pps_stateParameterKey)).toString();
        const QString callId = currentCallId(param);
        if (m_state != state) {
            m_state = state;
            emit q->stateChanged(m_state);
        }

            switch (m_state) {

                case ConnectedIdleState:
                {
                    if(m_state != oldState){
                        m_currentCallId.clear();
                        emit q->phoneReady(q->handsFreeServiceIdentifier());
                    }
                }
                break;
                case CallIncomingState:
                {
                    emit q->incomingCall(callId, q->handsFreeServiceIdentifier());
                }
                break;
                case CallActiveState:
                {
                    if(m_state != oldState){
                        emit q->callActivated(callId, q->handsFreeServiceIdentifier());
                        m_callDuration.start();
                        q->updateCallDuration();
                    }
                }
                break;
                case CallOutgoingDialingState:
                {
                    emit q->outgoingCall(callId, q->handsFreeServiceIdentifier());
                }
                case CallOutgoingAlertingState:
                {
                    emit q->outgoingCallAlerting(callId, q->handsFreeServiceIdentifier());
                }
                break;
                break;
                default:
                break;
            }


    }
}


Phone::Phone(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

Phone::State Phone::state() const
{
    return d->m_state;
}

QString Phone::activeCallId(const QString&) const
{
    // TODO: Read this from PPS, no need to store/cache separately.
    return d->m_currentCallId;
}

QString Phone::handsFreeServiceIdentifier()
{
    return QStringLiteral("SERVICE_HFP");
}

 int Phone::callDuration() const
 {
     return d->m_callDuration.elapsed();
 }

void Phone::accept(const QString&, const QString&)
{
    if (d->m_state != CallIncomingState)
        return;

    if (!d->m_ppsControlObject->setAttribute(QStringLiteral("command"), QPps::Variant(QStringLiteral("HFP_ACCEPT"))))
        qWarning() << "Phone::accept: Unable to accept call";
}

void Phone::dial(const QString &number, const QString&)
{
    if (number.isEmpty())
        return;

    d->m_lastNumber = number;
    d->m_currentCallId = number;

    if (d->m_state != ConnectedIdleState)
        return;

    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("HFP_CALL")));
    changeset.assignments.insert(QStringLiteral("cmd_data"), QPps::Variant(number));
    if (!d->m_ppsControlObject->setAttributes(changeset))
        qWarning() << "Phone::dial: Unable to dial";
}

void Phone::hangUp(const QString&, const QString&)
{
    if (d->m_state == ConnectedIdleState)
        return;

    if (!d->m_ppsControlObject->setAttribute(QStringLiteral("command"), QPps::Variant(QStringLiteral("HFP_HANGUP"))))
        qWarning() << "Phone::hangUp: Unable to hang up call";
}

void Phone::hold(const QString&, const QString&)
{
    //TODO: implement once PPS API is available
}

void Phone::mute(const QString&, const QString&)
{
    //TODO: implement once PPS API is available
}

void Phone::redial(const QString&)
{
    if (!d->m_lastNumber.isEmpty())
        dial(d->m_lastNumber);
}

void Phone::updateCallDuration()
{
    if (d->m_state == CallActiveState) {
        const int timeToNextSecond = 1000 - d->m_callDuration.elapsed() % 1000;
        d->m_callDurationTimer.start(timeToNextSecond + 100 /*safety margin for timer slack*/);
    } else {
        d->m_callDurationTimer.stop();
    }
    emit callDurationChanged();
}

}
