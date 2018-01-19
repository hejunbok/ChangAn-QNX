#ifndef QTQNXCAR2_RADIO_P_H
#define QTQNXCAR2_RADIO_P_H

#include "Radio.h"

#include <qpps/object.h>

#include <QTimer>

namespace QnxCar {

class Radio::Private : public QObject
{
    Q_OBJECT

public:
    Private(Radio *qq);

    Radio *q;

    Radio::Tuner m_tuner;
    QVariantList m_presetAmStations;
    double m_amStation;
    QVariantList m_presetFmStations;
    double m_fmStation;
    QVariantMap m_amTuner;
    QVariantMap m_fmTuner;
    QString m_song;
    QString m_artist;
    QString m_genre;
    QString m_stationName;
    bool m_highDefinitionEnabled;
    bool m_seekingUp;
    bool m_seekingDown;
    Radio::ScanDirection m_scanDirection;

    bool m_simulationModeEnabled;

    QTimer *m_scanTimer;

    QPps::Object *m_ppsControlObject;
    QPps::Object *m_ppsStatusObject;
    QPps::Object *m_ppsTunersObject;

    void simulateSeek(bool);
    void simulateTune(double);
    void simulateUpdatePreset(int index);
    void simulateWriteToStatusPps(const QVariantList &presets, double station);
    void stopSeeking();
    void programmaticSeekUp();
    void programmaticSeekDown();

private Q_SLOTS:
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void ppsTunersAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void scan();
};

}

#endif
