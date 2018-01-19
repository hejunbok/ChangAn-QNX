#include "VoiceInput.h"

VoiceInput::VoiceInput(QObject *parent)
   : QObject(parent),
     m_sessionEnded(true),
     m_state(DoneState),
     m_resultConfidence(0)
{
    connect(&m_voiceControl, &QnxCar::VoiceControl::stateChanged,
            this, &VoiceInput::ppsStateChanged);
    connect(&m_voiceControl, &QnxCar::VoiceControl::result,
            this, &VoiceInput::ppsResult);
}

VoiceInput::State VoiceInput::state() const
{
    return m_state;
}

int VoiceInput::resultConfidence() const
{
    return m_resultConfidence;
}

QString VoiceInput::resultText() const
{
    return m_resultText;
}

void VoiceInput::listen()
{
    m_sessionEnded = false;
    m_voiceControl.listen();
}

void VoiceInput::endListen()
{
    m_voiceControl.endListen();
}

void VoiceInput::cancel()
{
    m_sessionEnded = true;
    m_voiceControl.cancel();
    stateChangeInternal(DoneState);
}

void VoiceInput::ppsStateChanged(QnxCar::VoiceControl::State state)
{
    using namespace QnxCar;
    State newState = DoneState;
    switch (state) {
    case VoiceControl::PromptingState:
        newState = PromptingState;
        break;
    case VoiceControl::ListeningState:
        newState = ListeningState;
        break;
    case VoiceControl::ProcessingState:
        newState = ProcessingState;
        break;
    case VoiceControl::IdleState:
        newState = DoneState;
        break;
    case VoiceControl::RunningState:
    case VoiceControl::HeldState:
        qWarning("VoiceControl: unhandled state %d", state);
        return;
    }
    stateChangeInternal(newState);
}

void VoiceInput::ppsResult()
{
    m_resultConfidence = m_voiceControl.confidence();
    m_resultText = m_voiceControl.utterance();
    if (m_resultConfidence == 0) {
        // otherwise the voice recognition system would loop through the states as long as no
        // recognizable input is given
        cancel();
    }
    emit result();
}

void VoiceInput::stateChangeInternal(State newState)
{
    // when the session has ended, we can only go to higher states or abort right away
    // (this prevents glitches)
    if (m_sessionEnded && newState < m_state) {
        newState = DoneState;
    }
    if (m_state == newState) {
        return;
    }
    m_state = newState;
    emit stateChanged();
}
