#ifndef QTQNXCAR2_VOICECONTROL_H
#define QTQNXCAR2_VOICECONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Automated speech recognition (ASR)
 *
 * Allows controlling the ASR service and receiving
 * recognition results.
 *
 * Wrapper class for the /pps/services/asr/control object
 */
class QTQNXCAR2_EXPORT VoiceControl : public QObject
{
    Q_OBJECT

    /**
     * Voice input session state
     *
     * @accessors state()
     */
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    /**
     * State of voice recognition, pertaining to the last voice input
     *
     * @accessors speechState()
     */
    Q_PROPERTY(SpeechState speechState READ speechState NOTIFY speechStateChanged)

    /**
     * Confidence that the recognized text is correct (range 0-1000)
     *
     * @accessors confidence()
     * @sa result()
     */
    Q_PROPERTY(int confidence READ confidence)

    /**
     * Recognized text from last voice input
     *
     * @accessors utterance()
     * @sa result()
     */
    Q_PROPERTY(QString utterance READ utterance)

    Q_ENUMS(State)
    Q_ENUMS(SpeechState)

public:
    /// Describes the current state of the ASR manager
    enum State
    {
        IdleState,        ///< There is no speech session running
        ListeningState,   ///< Listening to user
        ProcessingState,  ///< Processing captured voice input
        PromptingState,   ///<
        RunningState,     ///< ???
        HeldState         ///< The speech session is on hold
    };

    /// Describes the current state of the voice recognition processing
    enum SpeechState
    {
        NoSpeech,         ///< There is currently no speech recognition active
        ProcessingSpeech, ///< The recorded voice is being processed
        HandledSpeech,    ///< The voice recognition has finished
        UnhandledSpeech   ///< (Not documented in PPS reference)
    };

    /**
     * Creates a VoiceControl object
     * @param parent parent object
     */
    explicit VoiceControl(QObject *parent = 0);

    /// #state
    State state() const;
    /// #speedState
    SpeechState speechState() const;
    /// #confidence
    int confidence() const;
    /// #utterance
    QString utterance() const;

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

Q_SIGNALS:
    void stateChanged(QnxCar::VoiceControl::State state);
    void speechStateChanged(QnxCar::VoiceControl::SpeechState speechState);
    void confidenceChanged(int confidence);
    void utteranceChanged(const QString &utterance);

    /**
     * Emitted when a recognition result has become available. The result is available as
     * #confidence and #utterance.
     */
    void result();

private:
    class Private;
    Private* const d;
};

}

#endif
