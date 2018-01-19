#include "VoiceControl.h"
#include "VoiceControl_p.h"

#include <QDebug>
#include <QJsonDocument>

namespace QnxCar {

static const char* pps_resultKey = "result";
static const char* pps_speechKey = "speech";
static const char* pps_stateKey = "state";
static const char* pps_strobeKey = "strobe";

static VoiceControl::State stringToState(const QString &stringData)
{
    if (stringData == QStringLiteral("idle"))
        return VoiceControl::IdleState;
    else if (stringData == QStringLiteral("listening"))
        return VoiceControl::ListeningState;
    else if (stringData == QStringLiteral("processing"))
        return VoiceControl::ProcessingState;
    else if (stringData == QStringLiteral("prompting"))
        return VoiceControl::PromptingState;
    else if (stringData == QStringLiteral("running"))
        return VoiceControl::RunningState;
    else if (stringData == QStringLiteral("held"))
        return VoiceControl::HeldState;
    else {
        qCritical("Unexpected voice control state \"%s\"", qPrintable(stringData));
        Q_ASSERT(false);
        return VoiceControl::IdleState;
    }
}

static VoiceControl::SpeechState attributeToSpeechState(const QPps::Variant &attribute)
{
    if (!attribute.isValid())
        return VoiceControl::NoSpeech;

    const QString stringData = attribute.toString();
    if (stringData == QStringLiteral("processing"))
        return VoiceControl::ProcessingSpeech;
    else if (stringData == QStringLiteral("handled"))
        return VoiceControl::HandledSpeech;
    else if (stringData == QStringLiteral("unhandled") || stringData == "unrecognized")
        return VoiceControl::UnhandledSpeech;
    else {
        qCritical("Unhandled speech state %s", qPrintable(stringData));
        Q_ASSERT(false);
        return VoiceControl::NoSpeech;
    }
}

static void attributeToResult(const QPps::Variant &attribute, int &confidence, QString &utterance)
{
    if (!attribute.isValid()) {
        confidence = 0;
        utterance.clear();
    } else {
        const QVariantMap result = attribute.toJson().toVariant().toMap();
        confidence = result[QStringLiteral("confidence")].toInt();
        utterance = result[QStringLiteral("utterance")].toString();
    }
}

VoiceControl::Private::Private(VoiceControl *qq)
    : QObject(qq)
    , q(qq)
    , m_state(IdleState)
    , m_speechState(NoSpeech)
    , m_confidence(0)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/services/asr/control"), false,
                                   QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        connect(m_ppsObject, SIGNAL(attributeRemoved(QString)),
                this, SLOT(ppsAttributeRemoved(QString)));

        m_ppsObject->setAttributeCacheEnabled(true);
        const QString stateString = m_ppsObject->attribute(QString::fromLatin1(pps_stateKey)).toString();
        if (!stateString.isNull()) //state is not set in some setups (e.g. TI J5)
            m_state = stringToState(stateString);
        m_speechState = attributeToSpeechState(m_ppsObject->attribute(QString::fromLatin1(pps_speechKey)));
        attributeToResult(m_ppsObject->attribute(QString::fromLatin1(pps_resultKey)), m_confidence, m_utterance);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void VoiceControl::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_stateKey)) {
        const VoiceControl::State state = stringToState(attribute.toString());
        if (m_state != state) {
            m_state = state;
            emit q->stateChanged(m_state);
        }
    } else if (name == QString::fromLatin1(pps_speechKey)) {
        const VoiceControl::SpeechState speechState = attributeToSpeechState(attribute);
        if (m_speechState != speechState) {
            m_speechState = speechState;
            emit q->speechStateChanged(m_speechState);
        }
    } else if (name == QString::fromLatin1(pps_resultKey)) {
        // "result" is effectively a signal, not a property. PPS, even in delta mode, forwards all
        // attribute writes, even if they don't change the value. Make sure to forward result
        // as a signal so that listeners are notified every time a result comes in.
        int confidence = 0;
        QString utterance;
        attributeToResult(attribute, confidence, utterance);

        m_confidence = confidence;
        m_utterance = utterance;

        emit q->result();
    }
}

void VoiceControl::Private::ppsAttributeRemoved(const QString &name)
{
    if (name == QString::fromLatin1(pps_speechKey)) {
        m_speechState = VoiceControl::NoSpeech;
        emit q->speechStateChanged(m_speechState);
    } else if (name == QString::fromLatin1(pps_resultKey)) {
        m_confidence = 0;
        m_utterance.clear();
    }
}


VoiceControl::VoiceControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

VoiceControl::State VoiceControl::state() const
{
    return d->m_state;
}

VoiceControl::SpeechState VoiceControl::speechState() const
{
    return d->m_speechState;
}

int VoiceControl::confidence() const
{
    return d->m_confidence;
}

QString VoiceControl::utterance() const
{
    return d->m_utterance;
}

void VoiceControl::listen()
{
    switch (d->m_state) {
        case IdleState:
            d->m_ppsObject->setAttribute(QString::fromLatin1(pps_strobeKey), QPps::Variant(QStringLiteral("on")));
            break;
        case PromptingState:
            d->m_ppsObject->setAttribute(QString::fromLatin1(pps_strobeKey), QPps::Variant(QStringLiteral("barge-in")));
            break;
        case HeldState:
            d->m_ppsObject->setAttribute(QString::fromLatin1(pps_strobeKey), QStringLiteral("release"));
            break;
        case ListeningState:
        case ProcessingState:
        case RunningState:
            break;
    }
}

void VoiceControl::endListen()
{
    if (d->m_state == ListeningState)
        d->m_ppsObject->setAttribute(QString::fromLatin1(pps_strobeKey), QStringLiteral("mic-off"));
    else
        cancel();
}

void VoiceControl::cancel()
{
    if (d->m_state == IdleState)
        return;

    d->m_ppsObject->setAttribute(QString::fromLatin1(pps_strobeKey), QPps::Variant(QStringLiteral("off")));
}

}
