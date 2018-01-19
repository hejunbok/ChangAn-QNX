#include "NetworkInfo.h"

#include <qpps/changeset.h>
#include <qpps/object.h>
#include <unistd.h>

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <QFile>

namespace {
static const QString networkControlDir = QStringLiteral("/pps/services/networking/control");
static const QString networkInterfacesDir = QStringLiteral("/pps/services/networking/all/interfaces/");
static const QString networkInfoDir = QStringLiteral("/pps/services/networking/all/status_public");

static const char *IP_REGEX = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
static const char *pps_defaultInterfaceKey = "default_interface";
static const char *pps_ipAddressKey = "ip_addresses";
static const char *pps_manualKey = "manual";
static const char *pps_gatewayKey = "ip_gateway";

void findIpNetmask(const QJsonArray &array, QString *ipAddress, QString *netmask)
{
    Q_ASSERT(ipAddress);
    Q_ASSERT(netmask);

    QRegularExpression regExp(QString::fromLatin1(IP_REGEX));

    for (int i = 0; i < array.size(); ++i) {
        const QStringList pieces = array.at(i).toString().split(QLatin1Literal("/"));
        Q_ASSERT(pieces.size() == 2);
        QRegularExpressionMatch regExpMatch = regExp.match(pieces[0]);
        if (regExpMatch.hasMatch()) {
            *ipAddress = pieces[0];
            *netmask = pieces[1];
        }
    }
}
}

namespace QnxCar {

class NetworkInfo::Private
{
public:
    Private() : dhcp(true), networkInfo(0) {}

    QString ipAddress;
    QString netmask;
    QString gateway;
    bool dhcp;
    QPps::Object *networkInfo;
    QPps::Object *networkControl;
    QString interfaceIdentifier;
};

NetworkInfo::NetworkInfo(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    while(!QFile::exists(networkInfoDir));
    while(!QFile::exists(networkControlDir));

    QPps::Object *networkInfo = new QPps::Object(networkInfoDir, QPps::Object::SubscribeMode, true, this);
    if (networkInfo->isValid()) {
        networkInfo->setAttributeCacheEnabled(true);
        connect(networkInfo, SIGNAL(attributeChanged(QString,QPps::Variant)), this, SLOT(updateInterfaceInfo(QString,QPps::Variant)));

        const QString defaultInterface = networkInfo->attribute(QLatin1Literal(pps_defaultInterfaceKey)).toString();

        while(!QFile::exists(networkInterfacesDir + defaultInterface));
        updateFromInterface(defaultInterface);
    }

    d->networkControl = new QPps::Object(networkControlDir, QPps::Object::PublishAndSubscribeMode, true, this);
    if (!d->networkControl->isValid()) {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << d->networkControl->errorString();
    } else {
        connect(d->networkControl, SIGNAL(attributeChanged(QString,QPps::Variant)), this, SLOT(onReply(QString,QPps::Variant)));
    }
}

NetworkInfo::~NetworkInfo()
{
    delete d;
}

QString NetworkInfo::ipAddress() const
{
    return d->ipAddress;
}

QString NetworkInfo::netmask() const
{
    return d->netmask;
}

QString NetworkInfo::gateway() const
{
    return d->gateway;
}

bool NetworkInfo::dhcp() const
{
    return d->dhcp;
}

void NetworkInfo::updateFromInterface(const QString &identifier)
{
    if (identifier != d->interfaceIdentifier) {
        delete d->networkInfo;
        d->networkInfo = 0;
    }

    if (!d->networkInfo) {
        d->networkInfo = new QPps::Object(networkInterfacesDir + identifier, QPps::Object::SubscribeMode, true, this);
        if (d->networkInfo->isValid()) {
            d->interfaceIdentifier = identifier;
            d->networkInfo->setAttributeCacheEnabled(true);
            connect(d->networkInfo, SIGNAL(attributeChanged(QString,QPps::Variant)), this, SLOT(updateNetworkInfo(QString, QPps::Variant)), Qt::UniqueConnection);
        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << d->networkInfo->errorString();
            delete d->networkInfo;
            d->networkInfo = 0;
            d->interfaceIdentifier.clear();
            return;
        }
    }

    if (d->networkInfo->isValid()) {
        QString ipAddress;
        QString netmask;
        findIpNetmask(d->networkInfo->attribute(QLatin1Literal(pps_ipAddressKey)).toJson().array(), &ipAddress, &netmask);

        const bool dhcp = d->networkInfo->attribute(QLatin1Literal(pps_manualKey)).toString() == QLatin1Literal("no");
        const QString gateway = d->networkInfo->attribute(QLatin1Literal(pps_gatewayKey)).toJson().array().at(0).toString();

        setIpAddress(ipAddress);
        setNetmask(netmask);
        setGateway(gateway);
        setDhcp(dhcp);
    }
}

void NetworkInfo::updateNetworkInfo(const QString &attribute, const QPps::Variant &value)
{
    if (attribute == QString::fromLatin1(pps_ipAddressKey)) {
        QString ipAddress;
        QString netmask;
        findIpNetmask(value.toJson().array(), &ipAddress, &netmask);
        setIpAddress(ipAddress);
        setNetmask(netmask);
    } else if (attribute == QString::fromLatin1(pps_manualKey)) {
        const bool dhcp = value.toString() == QLatin1Literal("no");
        setDhcp(dhcp);
    } else if (attribute == QString::fromLatin1(pps_gatewayKey)) {
        const QString gateway = value.toJson().array().at(0).toString();
        setGateway(gateway);
    }
}

void NetworkInfo::updateInterfaceInfo(const QString &attribute, const QPps::Variant &value)
{
    if (attribute == QString::fromLatin1(pps_defaultInterfaceKey))
        updateFromInterface(value.toString());
}

void NetworkInfo::setStaticNetworkConfig(const QString &ipAddress,const QString &netmask,const QString &gateway) {
    setDhcp(false);
    disconnectNetwork();
    sleep(2); // sleep 2 seconds
    connectNetwork(ipAddress,netmask,gateway);
    emit configurationSaved();
}

void NetworkInfo::setDynamicNetworkConfig() {
    setDhcp(true);
    disconnectNetwork();
    sleep(2); // sleep 2 seconds
    connectNetwork(QString(),QString(),QString());
    emit configurationSaved();
}

void NetworkInfo::setIpAddress(const QString &ipAddress)
{
    if (d->ipAddress == ipAddress)
        return;

    d->ipAddress = ipAddress;
    emit ipAddressChanged(d->ipAddress);
}

void NetworkInfo::setNetmask(const QString &netmask)
{
    if (d->netmask == netmask)
        return;

    d->netmask = netmask;
    emit netmaskChanged(d->netmask);
}

void NetworkInfo::setGateway(const QString &gateway)
{
    if (d->gateway == gateway)
        return;

    d->gateway = gateway;
    emit gatewayChanged(d->gateway);
}

void NetworkInfo::setDhcp(bool dhcp)
{
    if (d->dhcp == dhcp)
        return;

    d->dhcp = dhcp;
    emit dhcpChanged(d->dhcp);
}

void NetworkInfo::connectNetwork(const QString &ipAddress,const QString &netmask,const QString &gateway) {
    if (d->networkControl->isValid()) {
        QJsonObject json;
        QJsonArray jsonDat;

        if(dhcp()) {
            json["manual"] = QJsonValue(QStringLiteral("no"));
            json["manual6"] = QJsonValue(QStringLiteral("off"));
        } else if(!ipAddress.isNull() && !netmask.isNull() && !gateway.isNull()){
            json["manual"] = QJsonValue(QStringLiteral("yes"));
            json["manual6"] = QJsonValue(QStringLiteral("off"));
            json["ip_address"] = QJsonValue(ipAddress);
            json["netmask"] = QJsonValue(netmask);
            json["gateway"] = QJsonValue(gateway);
        } else {
            return; // there is not static info provided, nothing to save
        }

        jsonDat.append(d->interfaceIdentifier);
        jsonDat.append(json);

        QJsonDocument Doc;
        Doc.setArray(jsonDat);
        QString payload = Doc.toJson();

        payload = payload.replace(QRegExp("(\\r\\n|\\n|\\r|\\s)"), ""); // have to do some magck here, becasue JSON DOcument produces pretty printed JSON string whcih NET PPS doesnt like :(

        QPps::Changeset changeset;
        changeset.assignments.clear();
        changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("net_connected")));
        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(payload));
        changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(0));
        if (!d->networkControl->setAttributes(changeset)) {
            qWarning("Unable to write to network control: %s", qPrintable(d->networkControl->errorString()));
            return;
        }
    }
}

void NetworkInfo::disconnectNetwork(){
    if (d->networkControl->isValid()) {
        // hardcoded payload for now
        QPps::Changeset changeset;
        changeset.assignments.clear();
        changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("net_disconnected")));
        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant("\"" + d->interfaceIdentifier + "\""));
        changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(0));
        if (!d->networkControl->setAttributes(changeset)) {
            qWarning("Unable to write to network control: %s", qPrintable(d->networkControl->errorString()));
            return;
        }
    }
}


void NetworkInfo::onReply(const QString &attribute, const QPps::Variant &value)
{

}

}
