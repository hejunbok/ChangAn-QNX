#include "AudioControl.h"
#include "AudioControl_p.h"

#include <qpps/changeset.h>

#include <QDebug>
#include <QJsonDocument>

namespace QnxCar {

static const char* pps_speakerVolumeKey = "output.speaker.volume";
static const char* pps_handsetVolumeKey = "output.handset.volume";
static const char* pps_outputAvailableKey = "output.available";
static const char* pps_balanceKey = "balance";
static const char* pps_bassKey = "bass";
static const char* pps_fadeKey = "fade";
static const char* pps_midKey = "mid";
static const char* pps_trebleKey = "treble";

static AudioControl::DeviceType deviceFromString(const QString &s)
{
    if (s == QStringLiteral("handset"))
        return AudioControl::Handset;
    if (s == QStringLiteral("speaker"))
        return AudioControl::Speaker;

    qWarning("%s: Unexpected output device '%s'", Q_FUNC_INFO, qPrintable(s));
    Q_ASSERT(!"unhandled device type");
    return AudioControl::Speaker;
}

AudioControl::Private::Private(AudioControl *qq)
    : QObject(qq)
    , q(qq)
    , m_ppsMixerObject(0)
    , m_ppsStatusObject(0)
    , m_ppsControlObject(0)
    , m_ppsVolumeObject(0)
    , m_speakerVolume(60.0)
    , m_handsetVolume(60.0)
    , m_balance(50.0)
    , m_bass(74.0)
    , m_fade(42.0)
    , m_mid(92.0)
    , m_treble(79.0)
    , m_currentOutputDevice(AudioControl::Speaker)
{  
    m_ppsServicesDirWatcher = new QPps::DirWatcher(QStringLiteral("/pps/services/audio"), this);
    connect(m_ppsServicesDirWatcher, &QPps::DirWatcher::objectAdded, this, &Private::ppsServicesObjAdded);
}

void AudioControl::Private::ppsServicesObjAdded(const QString &object)
{
    if (object == QStringLiteral("control")){
        qWarning("%s: audio service is ready", Q_FUNC_INFO);

        // Delete the dir watcher since it's no longer needed (this also disconnects any signal connections)
        m_ppsServicesDirWatcher->deleteLater();

        // Open the mixer object
        m_ppsMixerObject = new QPps::Object(QStringLiteral("/pps/services/audio/mixer"),
                                            QPps::Object::PublishAndSubscribeMode, true, this);

        if (m_ppsMixerObject->isValid()) {
            connect(m_ppsMixerObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsMixerAttributeChanged(QString,QPps::Variant)));
            m_ppsMixerObject->setAttributeCacheEnabled(true);

            m_balance = m_ppsMixerObject->attribute(QString::fromLatin1(pps_balanceKey)).toDouble();
            m_bass = m_ppsMixerObject->attribute(QString::fromLatin1(pps_bassKey)).toDouble();
            m_fade = m_ppsMixerObject->attribute(QString::fromLatin1(pps_fadeKey)).toDouble();
            m_mid = m_ppsMixerObject->attribute(QString::fromLatin1(pps_midKey)).toDouble();
            m_treble = m_ppsMixerObject->attribute(QString::fromLatin1(pps_trebleKey)).toDouble();
        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsMixerObject->errorString();
        }

        // Open the status object
        m_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/services/audio/status"),
                                             QPps::Object::SubscribeMode, true, this);

        if (m_ppsStatusObject->isValid()) {
            connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsStatusAttributeChanged(QString,QPps::Variant)));
            m_ppsStatusObject->setAttributeCacheEnabled(true);

            m_currentOutputDevice = deviceFromString(m_ppsStatusObject->attribute(QString::fromLatin1(pps_outputAvailableKey)).toString());
            connectCurrentOutputVolumeChanged();

            m_speakerVolume = m_ppsStatusObject->attribute(QString::fromLatin1(pps_speakerVolumeKey)).toDouble();
            m_handsetVolume = m_ppsStatusObject->attribute(QString::fromLatin1(pps_handsetVolumeKey)).toDouble();

            emit q->speakerVolumeChanged(m_speakerVolume);
            emit q->handsetVolumeChanged(m_handsetVolume);

        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
        }

        // Open the control object
        m_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/audio/control"),
                                              QPps::Object::PublishMode, true, this);

        if (!m_ppsControlObject->isValid()) {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsControlObject->errorString();
        }

        // Open the volume object
        m_ppsVolumeObject = new QPps::Object(QStringLiteral("/pps/services/audio/volume"),
                                              QPps::Object::PublishMode, true, this);

        if (!m_ppsVolumeObject->isValid()) {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsVolumeObject->errorString();
        }
    }
}

void AudioControl::Private::ppsMixerAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_balanceKey)) {
        if (!qFuzzyCompare(m_balance, attribute.toDouble())) {
            m_balance = attribute.toDouble();
            emit q->balanceChanged(m_balance);
        }
    } else if (name == QString::fromLatin1(pps_bassKey)) {
        if (!qFuzzyCompare(m_bass, attribute.toDouble())) {
            m_bass = attribute.toDouble();
            emit q->bassChanged(m_bass);
        }
    } else if (name == QString::fromLatin1(pps_fadeKey)) {
        if (!qFuzzyCompare(m_fade, attribute.toDouble())) {
            m_fade = attribute.toDouble();
            emit q->fadeChanged(m_fade);
        }
    } else if (name == QString::fromLatin1(pps_midKey)) {
        if (!qFuzzyCompare(m_mid, attribute.toDouble())) {
            m_mid = attribute.toDouble();
            emit q->midChanged(m_mid);
        }
    } else if (name == QString::fromLatin1(pps_trebleKey)) {
        if (!qFuzzyCompare(m_treble, attribute.toDouble())) {
            m_treble = attribute.toDouble();
            emit q->trebleChanged(m_treble);
        }
    }
}

void AudioControl::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_speakerVolumeKey)) {
        if (!qFuzzyCompare(m_speakerVolume, attribute.toDouble())) {
            m_speakerVolume = attribute.toDouble();
            emit q->speakerVolumeChanged(m_speakerVolume);
        }
    } else if (name == QString::fromLatin1(pps_handsetVolumeKey)) {
        if (!qFuzzyCompare(m_handsetVolume, attribute.toDouble())) {
            m_handsetVolume = attribute.toDouble();
            emit q->handsetVolumeChanged(m_handsetVolume);
        }
    } else if (name == QString::fromLatin1(pps_outputAvailableKey)) {
        setCurrentOutputDevice(deviceFromString(attribute.toString()));
    }
}

bool AudioControl::Private::setVolume(const QString &output, double volume)
{
    QVariantMap map;
    map[QStringLiteral("name")] = output;
    map[QStringLiteral("level")] = volume;

    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(QStringLiteral("1")));
    changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("set_output_level")));
    changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(QJsonDocument::fromVariant(map)));
    printf("setVolume\n");
    if (!m_ppsControlObject->setAttributes(changeset)) {
        printf("AudioControl: unable to write back 'volume\n");
        qWarning() << "AudioControl: unable to write back 'volume'";
        return false;
    }

    if (!m_ppsVolumeObject->setAttributes(changeset)) {
        printf("AudioVolume: unable to write back 'volume\n");
        qWarning() << "AudioVolume: unable to write back 'volume'";
        return false;
    }

    return true;
}

void AudioControl::Private::connectCurrentOutputVolumeChanged()
{
    disconnect(q, &AudioControl::speakerVolumeChanged, q, &AudioControl::currentOutputVolumeChanged);
    disconnect(q, &AudioControl::handsetVolumeChanged, q, &AudioControl::currentOutputVolumeChanged);
    switch (m_currentOutputDevice) {
    case Speaker:
        connect(q, &AudioControl::speakerVolumeChanged, q, &AudioControl::currentOutputVolumeChanged);
        break;
    case Handset:
        connect(q, &AudioControl::handsetVolumeChanged, q, &AudioControl::currentOutputVolumeChanged);
        break;
    }
}

void AudioControl::Private::setCurrentOutputDevice(DeviceType type)
{
    if (m_currentOutputDevice == type)
        return;
    m_currentOutputDevice = type;
    connectCurrentOutputVolumeChanged();
    emit q->currentOutputDeviceChanged(m_currentOutputDevice);
    emit q->currentOutputVolumeChanged(q->currentOutputVolume());
}

AudioControl::AudioControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

double AudioControl::speakerVolume() const
{
    return d->m_speakerVolume;
}

void AudioControl::setSpeakerVolume(double rawVolume)
{
    const double volume = qBound(0.0, rawVolume, 100.0);

    if (qFuzzyCompare(d->m_speakerVolume, volume))
        return;

    d->setVolume(QStringLiteral("speaker"), volume);

    d->m_speakerVolume = volume;
    emit speakerVolumeChanged(d->m_speakerVolume);
}

double AudioControl::handsetVolume() const
{
    return d->m_handsetVolume;
}

void AudioControl::setHandsetVolume(double rawVolume)
{
    const double volume = qBound(0.0, rawVolume, 100.0);

    if (qFuzzyCompare(d->m_handsetVolume, volume))
        return;

    d->setVolume(QStringLiteral("handset"), volume);

    d->m_handsetVolume = volume;
    emit handsetVolumeChanged(d->m_handsetVolume);
}

double AudioControl::currentOutputVolume() const
{
    switch (d->m_currentOutputDevice) {
    case Speaker:
        return d->m_speakerVolume;
    case Handset:
        return d->m_handsetVolume;
    }

    Q_ASSERT(!"unhandled device type");

    return 0;
}

void AudioControl::setCurrentOutputVolume(double volume)
{
    switch (d->m_currentOutputDevice) {
    case Speaker:
        setSpeakerVolume(volume);
        break;
    case Handset:
        setHandsetVolume(volume);
        break;
    }
}

double AudioControl::balance() const
{
    return d->m_balance;
}

void AudioControl::setBalance(double rawBalance)
{
    const double balance = qBound(0.0, rawBalance, 100.0);

    if (qFuzzyCompare(d->m_balance, balance))
        return;

    if (!d->m_ppsMixerObject->setAttribute(QString::fromLatin1(pps_balanceKey), QPps::Variant(balance))) {
        qWarning() << "AudioControl: unable to write back 'balance'";
        return;
    }

    d->m_balance = balance;
    emit balanceChanged(d->m_balance);
}

double AudioControl::bass() const
{
    return d->m_bass;
}

void AudioControl::setBass(double rawBass)
{
    const double bass = qBound(0.0, rawBass, 100.0);

    if (qFuzzyCompare(d->m_bass, bass))
        return;

    if (!d->m_ppsMixerObject->setAttribute(QString::fromLatin1(pps_bassKey), QPps::Variant(bass))) {
        qWarning() << "AudioControl: unable to write back 'bass'";
        return;
    }

    d->m_bass = bass;
    emit bassChanged(d->m_bass);
}

double AudioControl::fade() const
{
    return d->m_fade;
}

void AudioControl::setFade(double rawFade)
{
    const double fade = qBound(0.0, rawFade, 100.0);

    if (qFuzzyCompare(d->m_fade, fade))
        return;

    if (!d->m_ppsMixerObject->setAttribute(QString::fromLatin1(pps_fadeKey), QPps::Variant(fade))) {
        qWarning() << "AudioControl: unable to write back 'fade'";
        return;
    }

    d->m_fade = fade;
    emit fadeChanged(d->m_fade);
}

double AudioControl::mid() const
{
    return d->m_mid;
}

void AudioControl::setMid(double rawMid)
{
    const double mid = qBound(0.0, rawMid, 100.0);

    if (qFuzzyCompare(d->m_mid, mid))
        return;

    if (!d->m_ppsMixerObject->setAttribute(QString::fromLatin1(pps_midKey), QPps::Variant(mid))) {
        qWarning() << "AudioControl: unable to write back 'mid'";
        return;
    }

    d->m_mid = mid;
    emit midChanged(d->m_mid);
}

double AudioControl::treble() const
{
    return d->m_treble;
}

void AudioControl::setTreble(double rawTreble)
{
    const double treble = qBound(0.0, rawTreble, 100.0);

    if (qFuzzyCompare(d->m_treble, treble))
        return;

    if (!d->m_ppsMixerObject->setAttribute(QString::fromLatin1(pps_trebleKey), QPps::Variant(treble))) {
        qWarning() << "AudioControl: unable to write back 'treble'";
        return;
    }

    d->m_treble = treble;
    emit trebleChanged(d->m_treble);
}

AudioControl::DeviceType AudioControl::currentOutputDevice() const
{
    return d->m_currentOutputDevice;
}

}
