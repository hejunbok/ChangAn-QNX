#ifndef QTQNXCAR2_AUDIOCONTROL_P_H
#define QTQNXCAR2_AUDIOCONTROL_P_H

#include "AudioControl.h"

#include <qpps/object.h>
#include "qpps/dirwatcher.h"
namespace QnxCar {

class AudioControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(AudioControl *qq);

    AudioControl *q;

    QPps::Object *m_ppsMixerObject;
    QPps::Object *m_ppsStatusObject;
    QPps::Object *m_ppsControlObject;
    QPps::Object *m_ppsVolumeObject;
    QPps::DirWatcher *m_ppsServicesDirWatcher;

    double m_speakerVolume;
    double m_handsetVolume;
    double m_balance;
    double m_bass;
    double m_fade;
    double m_mid;
    double m_treble;
    AudioControl::DeviceType m_currentOutputDevice;

    bool setVolume(const QString &output, double volume);
    void setCurrentOutputDevice(DeviceType type);
    void connectCurrentOutputVolumeChanged();

private Q_SLOTS:
    void ppsMixerAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void ppsServicesObjAdded(const QString &object);
};

}

#endif
