#ifndef VOICEINPUT_H
#define VOICEINPUT_H

#include <QObject>

#include "services/VoiceControl.h"

class VoiceInput : public QObject
{
    Q_OBJECT

    Q_PROPERTY(VoiceInput::State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int resultConfidence READ resultConfidence)
    Q_PROPERTY(QString resultText READ resultText)
    Q_ENUMS(State)

public:
    VoiceInput(QObject *parent = 0);

    enum State {
        PromptingState,
        ListeningState,
        ProcessingState,
        DoneState
    };

    State state() const;
    int resultConfidence() const;
    QString resultText() const;

Q_SIGNALS:
    void stateChanged();
    void result();

public Q_SLOTS:
    /**
     * Starts a voice recognition session.
     */
    void listen();

    /**
     * Stops voice recognition. Any already collected input is processed.
     */
    void endListen();

    /**
     * Cancels voice recognition. Any already collected input is discarded.
     */
    void cancel();

private Q_SLOTS:
    void ppsStateChanged(QnxCar::VoiceControl::State state);
    void ppsResult();

private:
    void stateChangeInternal(State newState);

    QnxCar::VoiceControl m_voiceControl;
    bool m_sessionEnded;
    State m_state;
    int m_resultConfidence;
    QString m_resultText;
};

#endif // VOICEINPUT_H
