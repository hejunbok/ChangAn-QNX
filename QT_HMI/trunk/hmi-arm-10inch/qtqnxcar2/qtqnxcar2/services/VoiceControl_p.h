#ifndef QTQNXCAR2_VOICECONTROL_P_H
#define QTQNXCAR2_VOICECONTROL_P_H

#include "VoiceControl.h"

#include <qpps/object.h>

namespace QnxCar {

class VoiceControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(VoiceControl *qq);

    VoiceControl *q;

    QPps::Object *m_ppsObject;

    State m_state;
    SpeechState m_speechState;
    int m_confidence;
    QString m_utterance;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void ppsAttributeRemoved(const QString &name);
};

}

#endif
