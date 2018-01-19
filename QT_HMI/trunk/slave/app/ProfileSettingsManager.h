#ifndef PROFILESETTINGSMANAGER_H
#define PROFILESETTINGSMANAGER_H

#include <qnxcar/ProfileManager.h>

namespace QnxCar {
    class AudioControl;
    class Hvac;
    class Radio;
}
class ProfileSettingsManager : public QnxCar::ProfileSettingsListener
{
public:
    void saveSettings(QMap<QString, QString> &settings) const;
    void restoreSettings(const QMap<QString, QString> &settings);

    void setAudioControl(QnxCar::AudioControl *audioControl);
    void setHvac(QnxCar::Hvac *hvac);
    void setRadio(QnxCar::Radio *radio);

private:
    QnxCar::AudioControl *m_audioControl;
    QnxCar::Hvac *m_hvac;
    QnxCar::Radio *m_radio;
};

#endif
