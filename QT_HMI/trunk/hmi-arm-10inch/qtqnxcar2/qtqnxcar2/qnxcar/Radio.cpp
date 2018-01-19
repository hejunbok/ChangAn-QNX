#include "Radio.h"
#include "Radio_p.h"

#include <QDebug>
#include <QJsonDocument>
#include <QTime>

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

namespace QnxCar {

static const char* pps_amKey = "am";
static const char* pps_artistKey = "artist";
static const char* pps_fmKey = "fm";
static const char* pps_genreKey = "genre";
static const char* pps_highDefinitionEnabledKey = "hd";
static const char* pps_songKey = "song";
static const char* pps_stationKey = "station";
static const char* pps_tunerKey = "tuner";

static const char* pps_amTunerKey = "am";
static const char* pps_fmTunerKey = "fm";
static const char* pps_seekStateKey = "seek";

static const int RADIO_DEFAULT_SCAN_INTERVAL = 3000; // ms

static void attributeToStations(const QPps::Variant &attribute, QVariantList &presetStations, double &station)
{
    const QJsonDocument document = attribute.toJson();
    const QVariantMap map = document.toVariant().toMap();

    presetStations = map[QStringLiteral("presets")].toList();
    station = map[QStringLiteral("station")].toDouble();
}

static Radio::Tuner stringToTuner(const QString &stringData)
{
    if (stringData == QStringLiteral("am"))
        return Radio::AmTuner;
    else
        return Radio::FmTuner;
}

static QString tunerToString(Radio::Tuner tuner)
{
    switch (tuner) {
    case Radio::AmTuner:
        return QStringLiteral("am");
    case Radio::FmTuner:
    default:
        return QStringLiteral("fm");
    }
}

Radio::Private::Private(Radio *qq)
    : QObject(qq)
    , q(qq)
    , m_tuner(FmTuner)
    , m_amStation(0)
    , m_fmStation(0)
    , m_highDefinitionEnabled(false)
    , m_seekingUp(false)
    , m_seekingDown(false)
    , m_scanDirection(Radio::ScanNone)
    , m_simulationModeEnabled(false)
    , m_scanTimer(new QTimer(this))
    , m_seek_state(tr("stop"))
{
    m_ppsControlObject = new QPps::Object(QStringLiteral("/pps/radio/control"),
                                          QPps::Object::PublishMode, true, this);
    m_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/radio/status"),
                                         QPps::Object::SubscribeMode, true, this);
    m_ppsTunersObject = new QPps::Object(QStringLiteral("/pps/radio/tuners"),
                                         QPps::Object::SubscribeMode, true, this);

    if (!m_ppsControlObject->isValid()) {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsControlObject->errorString();
    }

    if (m_ppsStatusObject->isValid()) {
        connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsStatusAttributeChanged(QString,QPps::Variant)));
        m_ppsStatusObject->setAttributeCacheEnabled(true);

        attributeToStations(m_ppsStatusObject->attribute(QString::fromLatin1(pps_amKey)), m_presetAmStations, m_amStation);
        attributeToStations(m_ppsStatusObject->attribute(QString::fromLatin1(pps_fmKey)), m_presetFmStations, m_fmStation);
        m_artist = m_ppsStatusObject->attribute(QString::fromLatin1(pps_artistKey)).toString();
        m_genre = m_ppsStatusObject->attribute(QString::fromLatin1(pps_genreKey)).toString();
        m_song = m_ppsStatusObject->attribute(QString::fromLatin1(pps_songKey)).toString();
        m_stationName = m_ppsStatusObject->attribute(QString::fromLatin1(pps_stationKey)).toString();
        m_highDefinitionEnabled = m_ppsStatusObject->attribute(QString::fromLatin1(pps_highDefinitionEnabledKey)).toBool();
        m_tuner = stringToTuner(m_ppsStatusObject->attribute(QString::fromLatin1(pps_tunerKey)).toString());
        m_seek_state = m_ppsStatusObject->attribute(QString::fromLatin1(pps_seekStateKey)).toString();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
    }

    if (m_ppsTunersObject->isValid()) {
        connect(m_ppsTunersObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsTunersAttributeChanged(QString,QPps::Variant)));
        m_ppsTunersObject->setAttributeCacheEnabled(true);

        m_amTuner = m_ppsTunersObject->attribute(QString::fromLatin1(pps_amTunerKey)).toJson().toVariant().toMap();
        m_fmTuner = m_ppsTunersObject->attribute(QString::fromLatin1(pps_fmTunerKey)).toJson().toVariant().toMap();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsTunersObject->errorString();
    }

    m_scanTimer->setInterval(RADIO_DEFAULT_SCAN_INTERVAL);
    connect(m_scanTimer, SIGNAL(timeout()), this, SLOT(scan()));
}

void Radio::Private::simulateSeek(bool upDirection)
{
    const QVariantMap &tunerInfo = (m_tuner == Radio::AmTuner ? m_amTuner : m_fmTuner);

    const double rangeMin = tunerInfo[QStringLiteral("rangeMin")].toDouble();
    const double rangeMax = tunerInfo[QStringLiteral("rangeMax")].toDouble();
    const double rangeStep = tunerInfo[QStringLiteral("rangeStep")].toDouble();

    const int stepCount = ((rangeMax - rangeMin) / rangeStep);

    int randomOffset = ((qrand()%100/100.0) / 5 * stepCount) + 1;
    if (!upDirection)
        randomOffset = -randomOffset;

    const double currentStation = (m_tuner == Radio::AmTuner ? m_amStation : m_fmStation);
    double targetStation = currentStation + (randomOffset * rangeStep);

    if (targetStation < rangeMin)
        targetStation = rangeMax - (rangeMin - targetStation);
    else if (targetStation > rangeMax)
        targetStation = rangeMin + (targetStation - rangeMax);

    q->tune(targetStation);
}

void Radio::Private::simulateWriteToStatusPps(const QVariantList &presets, double station)
{
    QVariantMap entry;
    entry[QStringLiteral("presets")] = presets;
    entry[QStringLiteral("station")] = station;

    QPps::Object *object = new QPps::Object(QStringLiteral("/pps/radio/status"), QPps::Object::PublishMode, true);
    object->setAttribute(tunerToString(m_tuner), QPps::Variant(QJsonDocument::fromVariant(entry)));
    object->deleteLater();
}

void Radio::Private::simulateTune(double station)
{
    const QVariantList presets = m_tuner == Radio::AmTuner ? m_presetAmStations : m_presetFmStations;
    simulateWriteToStatusPps(presets, station);
}

void Radio::Private::simulateUpdatePreset(int index)
{
    QVariantList presets = m_tuner == AmTuner ? m_presetAmStations : m_presetFmStations;
    const double station = q->station();
    presets[index] = station;
    simulateWriteToStatusPps(presets, station);
}

void Radio::Private::stopSeeking()
{
    if (m_seekingUp) {
        m_seekingUp = false;
        emit q->seekingUpChanged(m_seekingUp);
    }
    if (m_seekingDown) {
        m_seekingDown = false;
        emit q->seekingDownChanged(m_seekingDown);
    }
}

void Radio::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_amKey)) {
        QVariantList presetStations;
        double station;

        attributeToStations(attribute, presetStations, station);

        if (m_presetAmStations != presetStations) {
            m_presetAmStations = presetStations;
            if (m_tuner == AmTuner)
                emit q->presetStationsChanged(m_presetAmStations);
        }

        if (!qFuzzyCompare(m_amStation, station)) {
            m_amStation = station;
            if (m_tuner == AmTuner) {
                emit q->stationChanged(m_amStation);

                stopSeeking();
            }
        }
    } else if (name == QString::fromLatin1(pps_fmKey)) {
        QVariantList presetStations;
        double station;

        attributeToStations(attribute, presetStations, station);

        if (m_presetFmStations != presetStations) {
            m_presetFmStations = presetStations;
            if (m_tuner == FmTuner)
                emit q->presetStationsChanged(m_presetFmStations);
        }

        if (!qFuzzyCompare(m_fmStation, station)) {
            m_fmStation = station;
            if (m_tuner == FmTuner) {
                emit q->stationChanged(m_fmStation);

                stopSeeking();
            }
        }
    } else if (name == QString::fromLatin1(pps_artistKey)) {
        if (m_artist != attribute.toString()) {
            m_artist = attribute.toString();
            emit q->artistChanged(m_artist);
        }
    } else if (name == QString::fromLatin1(pps_genreKey)) {
        if (m_genre != attribute.toString()) {
            m_genre = attribute.toString();
            emit q->genreChanged(m_genre);
        }
    } else if (name == QString::fromLatin1(pps_songKey)) {
        if (m_song != attribute.toString()) {
            m_song = attribute.toString();
            emit q->songChanged(m_song);
        }
    } else if (name == QString::fromLatin1(pps_stationKey)) {
        if (m_stationName != attribute.toString()) {
            m_stationName = attribute.toString();
            emit q->stationNameChanged(m_stationName);
        }
    } else if (name == QString::fromLatin1(pps_highDefinitionEnabledKey)) {
        if (m_highDefinitionEnabled != attribute.toBool()) {
            m_highDefinitionEnabled = attribute.toBool();
            emit q->highDefinitionEnabledChanged(m_highDefinitionEnabled);
        }
    } else if (name == QString::fromLatin1(pps_tunerKey)) {
        const Tuner tuner = stringToTuner(attribute.toString());
        if (m_tuner != tuner) {
            m_tuner = tuner;
            emit q->tunerChanged(m_tuner);
            emit q->presetStationsChanged(m_tuner == Radio::AmTuner ? m_presetAmStations : m_presetFmStations);
            emit q->stationChanged(m_tuner == Radio::FmTuner ? m_amStation : m_fmStation);
        }
    } else if (name == QString::fromLatin1(pps_seekStateKey)) {
        if (m_seek_state != attribute.toString()) {
            m_seek_state = attribute.toString();
            emit q->seekStateChanged(m_seek_state);
        }
    }
}

void Radio::Private::ppsTunersAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_amTunerKey)) {
        m_amTuner = attribute.toJson().toVariant().toMap();
    } else if (name == QString::fromLatin1(pps_fmTunerKey)) {
        m_fmTuner = attribute.toJson().toVariant().toMap();
    }
}

void Radio::Private::scan()
{
    switch (m_scanDirection) {
        case Radio::ScanUp:
            programmaticSeekUp();
            break;
        case Radio::ScanDown:
            programmaticSeekDown();
            break;
        case Radio::ScanNone:
        default:
            break;
    }
}


Radio::Radio(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

Radio::Tuner Radio::tuner() const
{
    return d->m_tuner;
}

void Radio::setTuner(Tuner tuner)
{
    if (d->m_tuner == tuner)
        return;

    if (!d->m_ppsControlObject->setAttribute(QStringLiteral("tuner"), QPps::Variant(tunerToString(tuner)))) {
        qWarning() << "Radio: unable to write back: tuner";
        return;
    }

    d->m_tuner = tuner;
    emit tunerChanged(d->m_tuner);
    emit presetStationsChanged(d->m_tuner == AmTuner ? d->m_presetAmStations : d->m_presetFmStations);
    emit stationChanged(d->m_tuner == AmTuner ? d->m_amStation : d->m_fmStation);
}

QVariantList Radio::presetStations(Tuner tuner) const
{
    return tuner == AmTuner ? d->m_presetAmStations : d->m_presetFmStations;
}

QVariantList Radio::presetStations() const
{
    return presetStations(d->m_tuner);
}

void Radio::setPresetStations(Tuner tuner, const QVariantList& stations)
{
    (tuner == AmTuner ? d->m_presetAmStations :d->m_presetFmStations) = stations;
    if (tuner == d->m_tuner)
        emit presetStationsChanged(d->m_tuner == AmTuner ? d->m_presetAmStations : d->m_presetFmStations);
}

double Radio::station() const
{
    return (d->m_tuner == AmTuner ? d->m_amStation : d->m_fmStation);
}

QString Radio::song() const
{
    return d->m_song;
}

QString Radio::artist() const
{
    return d->m_artist;
}

QString Radio::genre() const
{
    return d->m_genre;
}

QString Radio::stationName() const
{
    return d->m_stationName;
}

bool Radio::highDefinitionEnabled() const
{
    return d->m_highDefinitionEnabled;
}

bool Radio::seekingUp() const
{
    return d->m_seekingUp;
}

bool Radio::seekingDown() const
{
    return d->m_seekingDown;
}

Radio::ScanDirection Radio::scanDirection() const
{
    return d->m_scanDirection;
}

double Radio::frequencyMinimum() const
{
    return (d->m_tuner == AmTuner ? d->m_amTuner[QStringLiteral("rangeMin")].toDouble()
                                  : d->m_fmTuner[QStringLiteral("rangeMin")].toDouble());
}

double Radio::frequencyMaximum() const
{
    return (d->m_tuner == AmTuner ? d->m_amTuner[QStringLiteral("rangeMax")].toDouble()
                                  : d->m_fmTuner[QStringLiteral("rangeMax")].toDouble());
}

double Radio::frequencyStep() const
{
    return (d->m_tuner == AmTuner ? d->m_amTuner[QStringLiteral("rangeStep")].toDouble()
                                  : d->m_fmTuner[QStringLiteral("rangeStep")].toDouble());
}

QString Radio::seekState() const
{
    return d->m_seek_state;
}

int Radio::scanInterval() const
{
    return d->m_scanTimer->interval();
}

void Radio::setScanInterval(int ms)
{
    d->m_scanTimer->setInterval(ms);
}

bool Radio::simulationModeEnabled() const
{
    return d->m_simulationModeEnabled;
}

void Radio::setSimulationModeEnabled(bool enabled)
{
    d->m_simulationModeEnabled = enabled;
    if (d->m_simulationModeEnabled)
        qsrand(QTime::currentTime().msec());
}

void Radio::Private::programmaticSeekUp()
{
    m_seekingUp = true;
    emit q->seekingUpChanged(m_seekingUp);

    if (m_simulationModeEnabled)
        simulateSeek(true);
    else
        m_ppsControlObject->setAttribute(QStringLiteral("seek"), QPps::Variant(QStringLiteral("up")));
}

void Radio::seekUp()
{
    stopScan();
    d->programmaticSeekUp();
}

void Radio::Private::programmaticSeekDown()
{
    m_seekingDown = true;
    emit q->seekingDownChanged(m_seekingDown);

    if (m_simulationModeEnabled)
        simulateSeek(false);
    else
        m_ppsControlObject->setAttribute(QStringLiteral("seek"), QPps::Variant(QStringLiteral("down")));
}

void Radio::seekDown()
{
    stopScan();
    d->programmaticSeekDown();

}

void Radio::stopSeek()
{
    d->stopSeeking();

    d->m_ppsControlObject->setAttribute(QStringLiteral("seek"), QPps::Variant(QStringLiteral("stop")));
}

void Radio::scanUp()
{
    if ((d->m_scanDirection == ScanUp) && d->m_scanTimer->isActive())
        return;

    d->m_scanTimer->stop();

    d->m_scanDirection = ScanUp;
    emit scanDirectionChanged(d->m_scanDirection);

    d->programmaticSeekUp();
    d->m_scanTimer->start();
}

void Radio::scanDown()
{
    if ((d->m_scanDirection == ScanDown) && d->m_scanTimer->isActive())
        return;

    d->m_scanTimer->stop();

    d->m_scanDirection = ScanDown;
    emit scanDirectionChanged(d->m_scanDirection);

    d->programmaticSeekDown();
    d->m_scanTimer->start();
}

void Radio::stopScan()
{
    if (!d->m_scanTimer->isActive())
        return;

    d->m_scanTimer->stop();

    d->m_scanDirection = ScanNone;
    emit scanDirectionChanged(d->m_scanDirection);
}

void Radio::tune(double rawStation)
{
    const QVariantMap &tunerInfo = (d->m_tuner == AmTuner ? d->m_amTuner : d->m_fmTuner);

    const double rangeMin = tunerInfo[QStringLiteral("rangeMin")].toDouble();
    const double rangeMax = tunerInfo[QStringLiteral("rangeMax")].toDouble();

    const double station = qBound(rangeMin, rawStation, rangeMax);

    if (d->m_simulationModeEnabled){
        d->simulateTune(station);
    }
    else{
        d->m_ppsControlObject->setAttribute(QStringLiteral("tune"), QPps::Variant(QString::number(station)));
    }
}

void Radio::updatePreset(int index)
{
    if (d->m_simulationModeEnabled) {
        d->simulateUpdatePreset(index);
    } else {
        //What to do? Don't see calls in the PPS API
    }
}

}
