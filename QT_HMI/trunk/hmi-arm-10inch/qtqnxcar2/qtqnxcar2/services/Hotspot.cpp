#include "Hotspot.h"

#include <qpps/changeset.h>
#include <qpps/object.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

namespace QnxCar {

    static const char* PPS_STATE_KEY= "state";
    static const char* PPS_RES_KEY = "res";
    static const char* PPS_DAT_KEY = "dat";
    static const char* PPS_ID_KEY = "id";
    static const char* PPS_ERR_KEY = "err";
    static const char* PPS_ERR_MEG_KEY = "errmsg";

    HotSpot::HotSpot(QObject *parent):QObject(parent) {

        // Init private variables with default values
        m_enabled = false;
        m_available = false;
        m_messageId = 0;
        m_curentName = QStringLiteral("");
        m_passphrase = QStringLiteral(""); // Password shoul be min 8 chars !
        m_securityType = QStringLiteral("wpa_mixed"); // TODO fix it, currently hardcoded
        m_band = QStringLiteral("bg");

        m_ppsServicesDirWatcher = new QPps::DirWatcher(QStringLiteral("/pps/services/tethering"), this);
        connect(m_ppsServicesDirWatcher, &QPps::DirWatcher::objectAdded, this, &HotSpot::initPPS);
    }

    void HotSpot::initPPS(const QString &object) {

       if (object == QStringLiteral("control")){
           // Delete the dir watcher since it's no longer needed (this also disconnects any signal connections)
           m_ppsServicesDirWatcher->deleteLater();

           // initialise PPS objects
           this->m_tm_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/tethering/control"),QPps::Object::PublishAndSubscribeMode, false, this);
           this->m_tm_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/services/tethering/status"),QPps::Object::PublishAndSubscribeMode, false, this);

           if (this->m_tm_ppsControlObject->isValid()) {
                connect(this->m_tm_ppsControlObject, SIGNAL(attributesChanged(QPps::Changeset)),this, SLOT(ppsTetherManReply(QPps::Changeset)));
                this->m_tm_ppsControlObject->setAttributeCacheEnabled(true);

                if (this->m_tm_ppsStatusObject->isValid()) {
                    connect(this->m_tm_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                            this, SLOT(ppsTetherManStatusChanged(QString,QPps::Variant)));
                    this->m_tm_ppsStatusObject->setAttributeCacheEnabled(true);

                    // all good - service is available
                    setAvailable(true);

                    // check the operational status of TetherMan
                    if(this->m_tm_ppsStatusObject->attribute(QString::fromLatin1(PPS_STATE_KEY)).toString() == QString::fromLatin1("ACTIVE")) {
                        setEnabled(true);
                    } else {
                        setEnabled(false);
                    }

                    retrieveProfile();
                } else {
                    qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << this->m_tm_ppsStatusObject->errorString();
                    setAvailable(false);
                }

            } else {
                qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << this->m_tm_ppsControlObject->errorString();
                setAvailable(false);
            }
        }
    }

    bool HotSpot::isAvailable() const {
        return this->m_available;
    }

    void HotSpot::setAvailable(bool value) {
        if(this->m_available != value) {
            this->m_available = value;
            emit availableChanged();
        }
    }

    QString HotSpot::getName() {
        return this->m_curentName;
    }

    void HotSpot::setName(const QString &value) {
        this->m_curentName = value;
        emit nameChanged();
    }

    QString HotSpot::getPassphrase() {
        return this->m_passphrase;
    }

    void HotSpot::setPassphrase(const QString &value) {
        this->m_passphrase = value;
        emit passwordChanged();
    }

    bool HotSpot::isEnabled() {
        return this->m_enabled;
    }

    void HotSpot::setEnabled(bool value) {
        this->m_enabled = value;
        emit enabledChanged();
    }

    QString HotSpot::getBand() {
        return m_band;
    }

    void HotSpot::setBand(const QString &value) {
        m_band = value;
        emit bandChanged();
    }

    void HotSpot::ppsTetherManStatusChanged(const QString &name, const QPps::Variant &attribute) {
        if (name == QString::fromLatin1(PPS_STATE_KEY)) {
            // check the status of TetherMan
            if(this->m_tm_ppsStatusObject->attribute(QString::fromLatin1(PPS_STATE_KEY)).toString() == QString::fromLatin1("ACTIVE")) {
                setEnabled(true);
            } else if(this->m_tm_ppsStatusObject->attribute(QString::fromLatin1(PPS_STATE_KEY)).toString() == QString::fromLatin1("INACTIVE")) {
                setEnabled(false);
            }
        }
    }

    void HotSpot::ppsTetherManReply(const QPps::Changeset &changes) {
        QMap<QString,QPps::Variant>::ConstIterator ppsIt;
        Message * message = new Message();
        for (ppsIt = changes.assignments.constBegin(); ppsIt != changes.assignments.constEnd(); ++ppsIt) {

            QString key = ppsIt.key();

            if(key == QString::fromLatin1(PPS_RES_KEY)) {
                message->setResponse(ppsIt.value().toString());
            } else if(key == QString::fromLatin1(PPS_ID_KEY)) {
                message->setId(ppsIt.value().toString());
            } else if(key == QString::fromLatin1(PPS_DAT_KEY)) {
                message->setData(ppsIt.value().toString());
            } else if(key == QString::fromLatin1(PPS_ERR_KEY)) {
                message->setErrorCode(ppsIt.value().toString());
            }
        }
        onTMControlReply(message);
    }

    void HotSpot::enableHotspot(bool value) {
        if(value) {
            startTethering();
        } else {

            stopTethering();
        }
    }

    void HotSpot::startTethering() {
        // signal thtat we starting hotspot
        emit hotspotStarting();

        if (this->m_curentName.length() == 0)
            return;

        // hardcoded payload for now
        QString payload = QStringLiteral(
                    "{\"lantype\":\"wlanap\",\"lancfg\":"
                        "{\"gateway_address\":\"192.168.0.1\",\"gateway_subnet\":\"255.255.255.0\",\"dhcp_lease_time\":120 },"
                        "\"wantype\":\"bridge\",\"lanprofile\":"
                    "{ \"ssid\":\"{1}\",\"band\":\"bg\",\"security_type\":\"wpa_mixed\",\"passphrase\":\"{2}\",\"inactivity_time\":\"15\",\"max_num_sta\":5,\"privacy\":true,\"block_conn\":false }"
                        "}");

        payload.replace("{1}", this->m_curentName);
        payload.replace("{2}", this->m_passphrase);

        QPps::Changeset changeset;
        changeset.assignments.clear();
        changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("start")));
        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(payload));
        changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(++this->m_messageId));

        if (!this->m_tm_ppsControlObject->setAttributes(changeset)) {
            qWarning("Unable to write to tetherman control: %s", qPrintable(this->m_tm_ppsControlObject->errorString()));
            return;
        }
    }

    void HotSpot::stopTethering() {
        // signal thtat we a=starting hotspot
        emit hotspotStopping();

        QPps::Changeset changeset;
        changeset.assignments.clear();
        changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("stop")));
        changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(++this->m_messageId));

        if (!this->m_tm_ppsControlObject->setAttributes(changeset)) {
            qWarning("Unable to write to tetherman control: %s", qPrintable(this->m_tm_ppsControlObject->errorString()));
            return;
        }
    }

    void HotSpot::retrieveProfile() {
        QString payload = QStringLiteral("{\"type\":\"lan\",\"lan_type\":\"wlanap\"}");

        QPps::Changeset changeset;
        changeset.assignments.clear();
        changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("retrieve_profile")));
        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(payload));
        changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(++this->m_messageId));

        if (!this->m_tm_ppsControlObject->setAttributes(changeset)) {
            qWarning("Unable to write to tetherman control: %s", qPrintable(this->m_tm_ppsControlObject->errorString()));
            return;
        }
    }

    void HotSpot::updateProfile() {
        // hardcoded payload for now
        QString payload = QStringLiteral("{\"type\":\"lan\",\"lantype\":\"wlanap\",\"wantype\":\"bridge\",\"lanprofile\":{\"ssid\":\"{1}\",\"band\":\"bg\",\"security_type\":\"wpa_mixed\",\"passphrase\":\"{2}\",\"max_num_sta\":5,\"privacy\":true, \"inactivity_time\":1,\"block_conn\":false}}");
        //"{\"type\":\"lan\",\"lan_type\":\"wlanap\",\"wantype\":\"bridge\",\"lanprofile\":{\"ssid\":\"{1}\",\"band\":\"bg\",\"security_type\":\"wpa_mixed\",\"passphrase\":\"{2}\",\"max_num_sta\":\"5\",\"autoshutdown_time\":\"15\",\"privacy\":\"true\",\"block_conn\":\"false\"}}");
        payload.replace("{1}", this->m_curentName);
        payload.replace("{2}", this->m_passphrase);

        QPps::Changeset changeset;
        changeset.assignments.clear();
        changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("update_profile")));
        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(payload));
        changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(++this->m_messageId));

        if (!this->m_tm_ppsControlObject->setAttributes(changeset)) {
            qWarning("Unable to write to tetherman control: %s", qPrintable(this->m_tm_ppsControlObject->errorString()));
            return;
        }

        emit profileUpdating();
    }

    void HotSpot::onTMControlReply(Message * message){

        if(message->getResponse() == "retrieve_profile") {
            if(message->getData().length() > 0) {
                // lets save profile locally
                m_curentProfile = message->getData();

                // populate profiles memmbers with values from PPS
                if(m_curentProfile.length() > 0) {
                    parseCurrentProfile(m_curentProfile);
                }
            }
        }

        if(message->getResponse() == "update_profile") {
            if(message->getErrorCode() == "0") {
                emit profileUpdated();
            }
        }
    }

    void HotSpot::parseCurrentProfile(QString profile) {
        QJsonObject json = QJsonDocument::fromJson(profile.toLocal8Bit()).object();
        QJsonObject lanprofile = json["lanprofile"].toObject();

        if(!lanprofile["ssid"].isUndefined()) {
            m_curentName = lanprofile["ssid"].toString();
            emit nameChanged();
        }

        if(!lanprofile["passphrase"].isUndefined()) {
            m_passphrase = lanprofile["passphrase"].toString();
            emit passwordChanged();
        }

        if(!lanprofile["band"].isUndefined()) {
            m_band = lanprofile["band"].toString();
            emit bandChanged();
        }
    }

    void HotSpot::onSaveProfile() {
        if(m_enabled && m_curentProfile.length() > 0) {
            updateProfile();
        } else {
            enableHotspot(true);
        }
    }

    void HotSpot::onEnableHotspot(bool value) {
        // name and password not set - we have to create new profile
        if(value && ( m_curentName.length() == 0 || m_passphrase.length() == 0)) {
            // disable manually, it will be enabled when config is successful
            enableHotspot(false);
            emit firstTimeConfig();
        } else {
            // name and password are set, lets start or stop HotSpot
            enableHotspot(value);
        }
    }
}
