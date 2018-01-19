#include "ProfileSettingsManager.h"

#include <qnxcar/Hvac.h>
#include <qnxcar/Radio.h>
#include <services/AudioControl.h>

#include <QMap>
#include <QStringList>

using namespace QnxCar;

static const QLatin1String audioVolumeKey("audio_volume");
static const QLatin1String audioBalanceKey("audio_balance");
static const QLatin1String audioBassKey("audio_bass");
static const QLatin1String audioFadeKey("audio_fade");
static const QLatin1String audioTrebleKey("audio_treble");

static const QLatin1String airCirculationSettingKey("hvac_air_circulation");
static const QLatin1String airConditioningEnabledKey("hvac_air_conditioning");
static const QLatin1String fanSettingLeftKey("hvac_fan_setting_l");
static const QLatin1String fanSettingRightKey("hvac_fan_setting_r");
static const QLatin1String fanSpeedLeftKey("hvac_fan_speed_l");
static const QLatin1String fanSpeedRightKey("hvac_fan_speed_r");
static const QLatin1String fanTemperatureLeftKey("hvac_fan_temp_l");
static const QLatin1String fanTemperatureRightKey("hvac_fan_temp_r");
static const QLatin1String heatedSeatLevelLeftKey("hvac_heated_seat_level_l");
static const QLatin1String heatedSeatLevelRightKey("hvac_heated_seat_level_r");
static const QLatin1String rearDefrostEnabledKey("hvac_rear_defrost");
static const QLatin1String zoneLinkEnabledKey("hvac_zone_link");

static const QLatin1String frontWindowHeatEnabledKey("hvac_frontwindow_heating");
static const QLatin1String backWindowHeatEnabledKey("hvac_backwindow_heating");
//static const QLatin1String outsideCircleEnabledKey("hvac_outside_circle");
//static const QLatin1String insideCircleEnabledKey("hvac_inside_circle");
static const QLatin1String airCondition_AC_modeEnabledKey("hvac_air_condition_ac_mode");
static const QLatin1String airConditionSwitchOnKey("hvac_air_condition_switch");

static const QLatin1String radioPresetAmKey("radio_preset_am");
static const QLatin1String radioPresetFmKey("radio_preset_fm");
static const QLatin1String radioPresetStationKey("radio_preset_station");
static const QLatin1String radioPresetTunerKey("radio_preset_tuner");

static bool readBool(const QString &s)
{
    return s == QStringLiteral("true");
}

static QString writeBool(bool b)
{
    return b ? QStringLiteral("true") : QStringLiteral("false");
}

static int readInt(const QString &s)
{
    bool ok;
    const int v = s.toInt(&ok);
    if (!ok)
        qCritical("Could not read int: %s", qPrintable(s));
    return v;
}

static double readDouble(const QString &s)
{
    bool ok;
    const double v = s.toDouble(&ok);
    if (!ok)
        qCritical("Could not read double: %s", qPrintable(s));
    return v;
}

static QVariantList readList(QString s)
{
    if (s.startsWith(QLatin1Char('[')))
        s = s.mid(1);
    else
        return QVariantList();
    if (s.endsWith(QLatin1Char(']')))
        s.chop(1);
    else
        return QVariantList();
    const QStringList split = s.split(QLatin1Char(','));
    QVariantList l;
    Q_FOREACH(const QString &i, split) {
        bool ok;
        l.append(i.toDouble(&ok));
        if (!ok)
            return QVariantList();
    }
    return l;
}

static QString writeList(const QVariantList &l)
{
    QStringList sl;
    sl.reserve(l.size());
    Q_FOREACH(const QVariant &i, l)
        sl.append(QString::number(i.toDouble()));
    const QString joined = sl.join(QLatin1Char(','));
    return QStringLiteral("[") + joined + QStringLiteral("]");
}

static QString writeTuner(Radio::Tuner tuner)
{
    switch (tuner) {
    case Radio::AmTuner:
        return QStringLiteral("am");
    case Radio::FmTuner:
        return QStringLiteral("fm");
    }

    Q_ASSERT(!"Unexpected Tuner value");
    return QString();
}

static Radio::Tuner readTuner(const QString &s)
{
    if (s == QStringLiteral("am"))
        return Radio::AmTuner;
    if (s == QStringLiteral("fm"))
        return Radio::FmTuner;

    return Radio::AmTuner;
}

void ProfileSettingsManager::saveSettings(QMap<QString, QString> &settings) const
{
    settings.insert(audioVolumeKey, QString::number(m_audioControl->speakerVolume()));
    settings.insert(audioBalanceKey, QString::number(m_audioControl->balance()));
    settings.insert(audioBassKey, QString::number(m_audioControl->bass()));
    settings.insert(audioFadeKey, QString::number(m_audioControl->fade()));
    settings.insert(audioTrebleKey, QString::number(m_audioControl->treble()));

    settings.insert(airCirculationSettingKey, writeBool(m_hvac->airCirculationSetting()));
    settings.insert(airConditioningEnabledKey, writeBool(m_hvac->airConditioningEnabled()));
    settings.insert(fanSettingLeftKey, QString::number(m_hvac->fanSettingLeft()));
    settings.insert(fanSettingRightKey, QString::number(m_hvac->fanSettingRight()));
    settings.insert(fanSpeedLeftKey, QString::number(m_hvac->fanSpeedLeft()));
    settings.insert(fanSpeedRightKey, QString::number(m_hvac->fanSpeedRight()));
    settings.insert(fanTemperatureLeftKey, QString::number(m_hvac->fanTemperatureLeft()));
    settings.insert(fanTemperatureRightKey, QString::number(m_hvac->fanTemperatureRight()));
    settings.insert(heatedSeatLevelLeftKey, QString::number(m_hvac->heatedSeatLevelLeft()));
    settings.insert(heatedSeatLevelRightKey, QString::number(m_hvac->heatedSeatLevelRight()));
    settings.insert(rearDefrostEnabledKey, writeBool(m_hvac->rearDefrostEnabled()));
    settings.insert(zoneLinkEnabledKey, writeBool(m_hvac->zoneLinkEnabled()));
    settings.insert(frontWindowHeatEnabledKey, writeBool(m_hvac->frontWindowHeatEnabled()));
    settings.insert(backWindowHeatEnabledKey, writeBool(m_hvac->backWindowHeatEnabled()));
//    settings.insert(outsideCircleEnabledKey, writeBool(m_hvac->outsideCircleEnabled()));
//    settings.insert(insideCircleEnabledKey, writeBool(m_hvac->insideCircleEnabled()));
    settings.insert(airCondition_AC_modeEnabledKey, writeBool(m_hvac->airCondition_AC_modeEnabled()));
    settings.insert(airConditionSwitchOnKey, writeBool(m_hvac->airConditionSwitchOn()));

    settings.insert(radioPresetAmKey, writeList(m_radio->presetStations(Radio::AmTuner)));
    settings.insert(radioPresetFmKey, writeList(m_radio->presetStations(Radio::FmTuner)));
    settings.insert(radioPresetTunerKey, writeTuner(m_radio->tuner()));
    settings.insert(radioPresetStationKey, QString::number(m_radio->station()));
}

void ProfileSettingsManager::restoreSettings(const QMap<QString, QString> &settings)
{
    QMap<QString, QString>::ConstIterator it;

    if ((it = settings.constFind(audioVolumeKey)) != settings.constEnd())
        m_audioControl->setSpeakerVolume(readDouble(it.value()));
    if ((it = settings.constFind(audioBalanceKey)) != settings.constEnd())
        m_audioControl->setBalance(readDouble(it.value()));
    if ((it = settings.constFind(audioBassKey)) != settings.constEnd())
        m_audioControl->setBass(readDouble(it.value()));
    if ((it = settings.constFind(audioFadeKey)) != settings.constEnd())
        m_audioControl->setFade(readDouble(it.value()));
    if ((it = settings.constFind(audioTrebleKey)) != settings.constEnd())
        m_audioControl->setTreble(readDouble(it.value()));

    if ((it = settings.constFind(airCirculationSettingKey)) != settings.constEnd())
        m_hvac->setAirCirculationSetting(readBool(it.value()));
    if ((it = settings.constFind(airConditioningEnabledKey)) != settings.constEnd())
        m_hvac->setAirConditioningEnabled(readBool(it.value()));
    if ((it = settings.constFind(fanSettingLeftKey)) != settings.constEnd())
        m_hvac->setFanSettingLeft(readInt(it.value()));
    if ((it = settings.constFind(fanSettingRightKey)) != settings.constEnd())
        m_hvac->setFanSettingRight(readInt(it.value()));
    if ((it = settings.constFind(fanSpeedRightKey)) != settings.constEnd())
        m_hvac->setFanSpeedRight(readInt(it.value()));
    if ((it = settings.constFind(fanSpeedLeftKey)) != settings.constEnd())
        m_hvac->setFanSpeedLeft(readInt(it.value()));
    if ((it = settings.constFind(fanTemperatureLeftKey)) != settings.constEnd())
        m_hvac->setFanTemperatureLeft(readInt(it.value()));
    if ((it = settings.constFind(fanTemperatureRightKey)) != settings.constEnd())
        m_hvac->setFanTemperatureRight(readInt(it.value()));
    if ((it = settings.constFind(heatedSeatLevelLeftKey)) != settings.constEnd())
        m_hvac->setHeatedSeatLevelLeft(readInt(it.value()));
    if ((it = settings.constFind(heatedSeatLevelRightKey)) != settings.constEnd())
        m_hvac->setHeatedSeatLevelRight(readInt(it.value()));
    if ((it = settings.constFind(rearDefrostEnabledKey)) != settings.constEnd())
        m_hvac->setRearDefrostEnabled(readBool(it.value()));
    if ((it = settings.constFind(zoneLinkEnabledKey)) != settings.constEnd())
        m_hvac->setZoneLinkEnabled(readBool(it.value()));
    if ((it = settings.constFind(frontWindowHeatEnabledKey)) != settings.constEnd())
        m_hvac->setFrontWindowHeatEnabled(readBool(it.value()));
    if ((it = settings.constFind(backWindowHeatEnabledKey)) != settings.constEnd())
        m_hvac->setBackWindowHeatEnabled(readBool(it.value()));
//    if ((it = settings.constFind(outsideCircleEnabledKey)) != settings.constEnd())
//        m_hvac->setOutsideCircleEnabled(readBool(it.value()));
//    if ((it = settings.constFind(insideCircleEnabledKey)) != settings.constEnd())
//        m_hvac->setInsideCircleEnabled(readBool(it.value()));
    if ((it = settings.constFind(airCondition_AC_modeEnabledKey)) != settings.constEnd())
        m_hvac->setAirCondition_AC_modeEnabled(readBool(it.value()));
    if ((it = settings.constFind(airConditionSwitchOnKey)) != settings.constEnd())
        m_hvac->setAirConditionSwitchOn(readBool(it.value()));

    if ((it = settings.constFind(radioPresetAmKey)) != settings.constEnd())
        m_radio->setPresetStations(Radio::AmTuner, readList(it.value()));
    if ((it = settings.constFind(radioPresetFmKey)) != settings.constEnd())
        m_radio->setPresetStations(Radio::FmTuner, readList(it.value()));
    if ((it = settings.constFind(radioPresetTunerKey)) != settings.constEnd())
        m_radio->setTuner(readTuner(it.value()));
    if ((it = settings.constFind(radioPresetStationKey)) != settings.constEnd())
        m_radio->tune(readDouble(it.value()));
}

void ProfileSettingsManager::setAudioControl(AudioControl *audioControl)
{
    m_audioControl = audioControl;
}

void ProfileSettingsManager::setHvac(Hvac *hvac)
{
    m_hvac = hvac;
}

void ProfileSettingsManager::setRadio(Radio *radio)
{
    m_radio = radio;
}

