#include "Wifi.h"
#include "Wifi_p.h"
#include <qpps/changeset.h>
#include <qpps/object.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using namespace QnxCar;

static const char* PPS_SCAN_RESULTS_KEY = "scan_results";
static const char* PPS_WIFI_POWER_KEY = "wifi_power";
static const char* PPS_WIFI_CONNECTED_KEY = "wifi_connected";
static const char* PPS_WIFI_STATUS_KEY = "wifi_status";
static const char* PPS_WIFI_LAST_EVENT_KEY = "last_event";

static const char* WPA_EVENT_TEMP_DISABLED = "CTRL-EVENT-SSID-TEMP-DISABLED";

static const char* WIFI_KEY_MNGT_NONE = "NONE";
static const char* WIFI_KEY_MNGT_WPA_PSK = "WPA-PSK";

Wifi::Private::Private(Wifi *qq):QObject(qq), q(qq), m_available(false) {
    accessPoints = new WifiAPModel(q->parent());
    m_ppsServicesDirWatcher = new QPps::DirWatcher(QStringLiteral("/pps/services/wifi"), this);
    connect(m_ppsServicesDirWatcher, &QPps::DirWatcher::objectAdded, this, &Private::initPPS);
}

void Wifi::Private::initPPS(const QString &object) {
     if (object == QStringLiteral("control")){
        // Delete the dir watcher since it's no longer needed (this also disconnects any signal connections)
        m_ppsServicesDirWatcher->deleteLater();

        // setup scan timer
        scanTimer = new QTimer(this);
        connect(scanTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

        this->m_wf_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/wifi/control"),QPps::Object::PublishAndSubscribeMode, false, this);
        this->m_wf_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/services/wifi/status"),QPps::Object::PublishAndSubscribeMode, false, this);

        if (this->m_wf_ppsControlObject->isValid()) {

            if (this->m_wf_ppsStatusObject->isValid()) {

                connect(m_wf_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                        this, SLOT(ppsStatusAttributeChanged(QString,QPps::Variant)));

                this->m_wf_ppsStatusObject->setAttributeCacheEnabled(true);

                if(this->m_wf_ppsStatusObject->attribute(QString::fromLatin1(PPS_WIFI_POWER_KEY)).toString() == QString::fromLatin1("on")) {
                    m_powered = true;
                    doScan();
                } else if(this->m_wf_ppsStatusObject->attribute(QString::fromLatin1(PPS_WIFI_POWER_KEY)).toString() == QString::fromLatin1("off")) {
                    m_powered = false;
                }

                if(this->m_wf_ppsStatusObject->attribute(QString::fromLatin1(PPS_WIFI_CONNECTED_KEY)).toString() == QString::fromLatin1("true")) {
                    m_connected = true;
                    QPps::Variant attribute = this->m_wf_ppsStatusObject->attribute(QString::fromLatin1(PPS_WIFI_STATUS_KEY));
                    if(attribute.isValid()) {
                        processStatusResult(attribute);
                    }
                } else if(this->m_wf_ppsStatusObject->attribute(QString::fromLatin1(PPS_WIFI_POWER_KEY)).toString() == QString::fromLatin1("false")) {
                    m_connected = false;
                }

                // all good - service is available
                setAvailable(true);
            } else {
                qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << this->m_wf_ppsStatusObject->errorString();
                setAvailable(false);
            }
        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << this->m_wf_ppsControlObject->errorString();
            setAvailable(false);
        }

        this->m_messageId = 0;
     }
}

void Wifi::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute) {

    // TODO Need to add filtes here to filter specific attributes changes Wifi Work still in progress
    // look for scan result
    if(name == QString::fromLatin1(PPS_SCAN_RESULTS_KEY)) {
        processScanResult(attribute);
    }

    // grab the last_event attribute
    if(name == QString::fromLatin1(PPS_WIFI_LAST_EVENT_KEY)) {
        QString s_attribute;

        if(attribute.isValid()) {
            s_attribute = attribute.toString();
        }
        // check if attribute contains error event message
        if(!s_attribute.isEmpty() && s_attribute.contains(QString::fromLatin1(WPA_EVENT_TEMP_DISABLED))) {
            // fire auth failed signal
            emit q->failedConnect();
            // disable current operation with network
            doDisable();
        }
    }

    if(name == QString::fromLatin1(PPS_WIFI_CONNECTED_KEY)) {
        if(attribute == "true") {
            m_connected = true;
        } else {
            m_connected = false;
        }
        emit q->connectedChanged();
    }

    if(name == QString::fromLatin1(PPS_WIFI_POWER_KEY)) {
        if(attribute == QString::fromLatin1("on") && m_powered == false) {
            m_powered = true;

            emit q->poweredChanged();

        } else if(attribute == QString::fromLatin1("off") && m_powered == true) {
            m_powered = false;

            // stop scanning
            if(scanTimer && scanTimer->isActive())
                scanTimer->stop();

            emit q->poweredChanged();
        }
    }

    if(name == QString::fromLatin1(PPS_WIFI_STATUS_KEY) && attribute.toString().length() > 0) {
        if(m_connected) {
            processStatusResult(attribute);
            emit q->successfulConnect();
        }
    }
    emit q->statusChanged();
}

void Wifi::Private::doStart() {
    //TODO check if WIFI is already ON, if yes - skip
    QPps::Changeset changeset;
    changeset.assignments.clear();
    changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("wifi_power")));
    changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(QStringLiteral("on")));
    changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(++this->m_messageId));

    if (!this->m_wf_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to wifi control: %s", qPrintable(this->m_wf_ppsControlObject->errorString()));
        return;
    }
}

void Wifi::Private::doStop() {
    QPps::Changeset changeset;
    changeset.assignments.clear();
    changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("wifi_power")));
    changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(QStringLiteral("off")));
    changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(++this->m_messageId));

    if (!this->m_wf_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to wifi control: %s", qPrintable(this->m_wf_ppsControlObject->errorString()));
        return;
    }

    // check if timer is scaning, if YES stop it
    if(scanTimer && scanTimer->isActive())
        scanTimer->stop();
}

void Wifi::Private::doStartScan() {
    if(scanTimer && m_powered)
        scanTimer->start(2000);
}

void Wifi::Private::doStopScan() {
    if(scanTimer && scanTimer->isActive())
        scanTimer->stop();
}

ControlCommand* Wifi::Private::doScan() {
    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(this->m_wf_ppsControlObject, this);
    command->setMessage(QStringLiteral("scan"));
    connect(command, &QnxCar::ControlCommand::finished, this, &Wifi::Private::onReply);
    command->start();
    return command;
}

ControlCommand* Wifi::Private::doAdd(QString bssid, QString identity, QString password) {

    emit q->connectingToNetwork();
    // find reference to WifiAccessPoint in the model using bssid
    WifiAccessPoint ap = accessPoints->findDeviceByAddress(bssid);
    m_apName = ap.ssid;
    emit q->apNameChanged();

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(this->m_wf_ppsControlObject, this);
    command->setMessage(QStringLiteral("wnet_new"));

    QJsonObject dat;
    dat.insert(QStringLiteral("ssid"), ap.ssid);
    dat.insert(QStringLiteral("key_mgmt"), ap.key_mgmt);
    // provide only when WPA-PSK
    if(ap.key_mgmt == QString::fromLatin1(WIFI_KEY_MNGT_WPA_PSK)) {
        dat.insert(QStringLiteral("psk"), password);
    }

    command->setDat(dat);
    connect(command, &QnxCar::ControlCommand::finished, this, &Wifi::Private::onAddReply);
    command->start();
    return command;
}

ControlCommand* Wifi::Private::doEnable(QString nid) {
    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(this->m_wf_ppsControlObject, this);
    command->setMessage(QStringLiteral("wnet"));
    command->setDat(QPps::Variant(nid));
    connect(command, &QnxCar::ControlCommand::finished, this, &Wifi::Private::onEnableReply);
    command->start();
    return command;
}

ControlCommand* Wifi::Private::doSelect(QString nid) {
    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(this->m_wf_ppsControlObject, this);
    command->setMessage(QStringLiteral("wnet_select"));
    command->setDat(QPps::Variant(nid));
    connect(command, &QnxCar::ControlCommand::finished, this, &Wifi::Private::onSelectReply);
    command->start();
    return command;
}

ControlCommand* Wifi::Private::doDisable() {
    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(this->m_wf_ppsControlObject, this);
    command->setMessage(QStringLiteral("wnet_disableall"));
    connect(command, &QnxCar::ControlCommand::finished, this, &Wifi::Private::onDisableReply);
    command->start();
    return command;
}

ControlCommand* Wifi::Private::requestScanResults() {
    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(this->m_wf_ppsControlObject, this);
    command->setMessage(QStringLiteral("scan_result"));
    connect(command, &QnxCar::ControlCommand::finished, this, &Wifi::Private::onScanResultReady);
    command->start();
    return command;
}

void Wifi::Private::processScanResult(const QPps::Variant &attribute) {
    // cleanup model
    accessPoints->removeAllDevices();

    QJsonArray jsonArr = QJsonDocument::fromJson(attribute.toString().toLocal8Bit()).array();
    foreach (const QJsonValue & value, jsonArr) {
        QJsonObject obj = value.toObject();
        WifiAccessPoint wap;
        QJsonArray flags = obj["flags"].toArray();

        foreach (const QJsonValue & value2, flags) {
            if(value2.toString().contains(QString("PSK"))) {
                wap.key_mgmt = QString("WPA-PSK");
                wap.secure = true;
                break;
            }
        }

        // we assume this is unsecure network if nothing was set previously
        if(wap.key_mgmt.length() == 0) {
            if(flags.size() == 1 && flags.at(0).toString().contains(QString("ESS"))) {
                wap.key_mgmt = QString("NONE");
                wap.secure = false;
            } else {
                continue;   // do not add this and get next one, becasue we wont be able to authenticate anyways
            }
        }

        //extract fields from the object
        wap.bssid = obj["bssid"].toString();
        wap.connected = false;
        wap.ssid = obj["ssid"].toString();
        wap.name = obj["ssid"].toString();
        wap.signalLevel = obj["signal_level"].toString();
        wap.frequency = obj["frequency"].toString();

        accessPoints->addDevice(wap);
    }
}

void Wifi::Private::processStatusResult(const QPps::Variant &attribute) {
    QJsonObject jsonObj = QJsonDocument::fromJson(attribute.toString().toLocal8Bit()).object();
    QString bssid = jsonObj["bssid"].toString();
    QString wpa_state = jsonObj["wpa_state"].toString();
    QString ip_address = jsonObj["ip_address"].toString();
    QString mac_address = jsonObj["address"].toString();

    if(bssid.length() > 0 && wpa_state == QStringLiteral("COMPLETED")) {

        WifiAccessPoint wap = accessPoints->findDeviceByAddress(bssid);
        if(wap.bssid.length() > 0) {
            wap.connected = true;
            wap.ip_address = ip_address;
            wap.address = mac_address;

            // update current device name
            m_apName = wap.ssid;
            emit q->apNameChanged();

            // re-add device, function will take care of proper updating and notifying
            accessPoints->addDevice(wap);
        } else {
            qWarning() << Q_FUNC_INFO << "No BSSID specified in search result";
        }
    } else {
        qWarning() << Q_FUNC_INFO << "No BSSID specified in wifi_status attribute";
    }
}

void Wifi::Private::onTimer() {
    doScan();
}

void Wifi::Private::onReply(QnxCar::ControlCommand *command) {
    if (command->hasError()) {
        const QString errorString = command->errorString();
        //report error here
        return;
    }
    emit q->scanSuccess();
}

void Wifi::Private::onScanResultReady(QnxCar::ControlCommand *command) {
    if (command->hasError()) {
        const QString errorString = command->errorString();
        //report error here
        return;
    }
    emit q->scanResultSuccess();
}

void Wifi::Private::onAddReply(QnxCar::ControlCommand *command) {
    if (command->hasError()) {
        const QString errorString = command->errorString();
        //report error here
        emit q->failedConnect(); // TODO add Error Message
        return;
    }

    QString dat = command->datResponse().toString();

    // after succ add, we need to enable this network
    doSelect(dat);
}

void Wifi::Private::onEnableReply(QnxCar::ControlCommand *command) {
    if (command->hasError()) {
        const QString errorString = command->errorString();
        //report error here
        emit q->failedConnect(); // TODO add Error Message
        return;
    }
}

void Wifi::Private::onSelectReply(QnxCar::ControlCommand *command) {
    if (command->hasError()) {
        const QString errorString = command->errorString();
        //report error here
        emit q->failedConnect(); // TODO add Error Message
        return;
    }
}

void Wifi::Private::onDisableReply(QnxCar::ControlCommand *command) {
    if (command->hasError()) {
        const QString errorString = command->errorString();
        //report error here
        emit q->failedConnect();
        return;
    }
}

void Wifi::Private::setAvailable(bool value)
{
    if(m_available != value) {
        m_available = value;
        emit q->availableChanged();
    }
}

Wifi::Private::~Private() {}

Wifi::Wifi(QObject *parent):QObject(parent),d(new Private(this)) {
    d->m_connected = false;
}

bool Wifi::isAvailable() const {
    return d->m_available;
}

void Wifi::startWifi() {
    d->doStart();
}

void Wifi::stopWifi() {
    d->doStop();
}

void Wifi::startScan() {
    d->doStartScan();
}

void Wifi::stopScan() {
    d->doStopScan();
}

void Wifi::connect(QString bssid, QString identity, QString password) {
    d->doAdd(bssid, identity, password);
}

void Wifi::disconnect() {
    d->doDisable();
}

void Wifi::cancel() {
}

bool Wifi::connected() {
    return d->m_connected;
}

bool Wifi::powered() {
    return d->m_powered;
}

QString Wifi::apName() {
    return d->m_apName;
}

void  Wifi::status() {
}

WifiAPModel*  Wifi::accessPoints() const {
    return d->accessPoints;
}

/* Implementation of Message class */

Message::Message(QObject *parent):QObject(parent) {
}

QString Message::getMessage() {
    return m_msg;
}

QString Message::getData() {
    return m_dat;
}

QString Message::getId() {
    return m_id;
}

QString Message::getResponse() {
    return m_res;
}

QString Message::getErrorCode() {
    return m_err;
}

QString Message::getErrorString() {
    return m_errstr;
}

void Message::setMessage(QString value) {
    m_msg = value;
}

void Message::setData(QString value) {
    m_dat = value;
}

void Message::setId(QString value) {
    m_id = value;
}

void Message::setResponse(QString value) {
    m_res = value;
}

void Message::setErrorCode(QString value) {
    m_err = value;
}

void Message::setErrorString(QString value) {
    m_errstr = value;
}
