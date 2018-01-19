#include "SystemInfo.h"

#include <QString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <qpps/object.h>

namespace QnxCar {

static const QString systemInfoDir = QStringLiteral("/pps/qnxcar/system/");
static const QString updateStatusDir = QStringLiteral("/pps/services/update/status");
static const QString updateControlDir = QStringLiteral("/pps/services/update/control");

static const char* pps_buildNumberKey = "buildNum";
static const char* pps_revisionKey = "car2Rev";
static const char* pps_buildDateKey = "date";

static const char* pps_updateAvailableKey = "updateAvailable";
static const char* pps_updateDetailsKey = "updateDetails";
static const char* pps_updateErrorKey = "updateError";
static const char* pps_updateCmdKey = "cmd";

class SystemInfo::Private: public QObject
{
public:
    QString buildNumber;
    QString revision;
    QString buildDate;
    bool updateAvailable;
    QString updateDetails;
    QString updateError;
    bool m_available;

    QPps::Object *updateControl;
    explicit Private(QObject *parent = 0);
};

SystemInfo::Private::Private(QObject *parent)
    : QObject(parent)
    , m_available(false)
    , updateAvailable(false)
{}

SystemInfo::SystemInfo(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->updateControl = new QPps::Object(updateControlDir, QPps::Object::PublishMode, true, this);

    qWarning() << "Init PPS";

    if(d->updateControl->isValid()) {
        d->updateControl->setAttribute(pps_updateCmdKey, 1); // Check for update

        qWarning() << "Opened Control object";

        QPps::Object *updateInfo = new QPps::Object(updateStatusDir, QPps::Object::SubscribeMode, true, this);
        updateInfo->setAttributeCacheEnabled(true);

        if (updateInfo->isValid()) {

            qWarning() << "Opened Status object";

            connect(updateInfo, SIGNAL(attributesChanged(QPps::Changeset)), this, SLOT(handleUpdateInfoChanged(QPps::Changeset)));
            // check if update availabla right on the begining
            d->updateAvailable = updateInfo->attribute(QString::fromLatin1(pps_updateAvailableKey)).toInt() != 0;
            d->updateDetails = updateInfo->attribute(QString::fromLatin1(pps_updateDetailsKey)).toString();
            d->updateError = updateInfo->attribute(QString::fromLatin1(pps_updateErrorKey)).toString();

            // if all initialisation passed, set available to true
            setAvailable(true);

        } else {
            qWarning() << "Failed to get update information: " << updateInfo->errorString();
            setAvailable(false);
        }

    } else {
        qWarning() << "Failed to open control object: " << d->updateControl->errorString();
        setAvailable(false);
    }

    // check separately for System Info (it is prepopulated so it shoul be available on system)
    QPps::Object systemInfo(systemInfoDir + QStringLiteral("info"), QPps::Object::SubscribeMode, this);
    systemInfo.setAttributeCacheEnabled(true);

    if (systemInfo.isValid()) {
        d->buildNumber = systemInfo.attribute(QString::fromLatin1(pps_buildNumberKey)).toString();
        d->revision = systemInfo.attribute(QString::fromLatin1(pps_revisionKey)).toString();
        d->buildDate = systemInfo.attribute(QString::fromLatin1(pps_buildDateKey)).toString();
    } else {
        qWarning() << "Failed to get system information: " << systemInfo.errorString();
        setAvailable(false);
    }
}

SystemInfo::~SystemInfo()
{
    delete d;
}

QString SystemInfo::buildNumber() const
{
    return d->buildNumber;
}

QString SystemInfo::revision() const
{
    return d->revision;
}

QString SystemInfo::buildDate() const
{
    return d->buildDate;
}

bool SystemInfo::updateAvailable() const
{
    return d->updateAvailable;
}

QString SystemInfo::updateDetails() const{
    return d->updateDetails;
}

QString SystemInfo::updateError() const{
    return d->updateError;
}

bool SystemInfo::isAvailable() const {
    return d->m_available;
}

void SystemInfo::performUpdate()
{
    d->updateControl->setAttribute(QString::fromLatin1(pps_updateCmdKey), 2);
}

void SystemInfo::handleUpdateInfoChanged(const QPps::Changeset &changes) {
    QMap<QString,QPps::Variant>::ConstIterator ppsIt;

    QJsonObject json;
    QJsonDocument doc;

    for (ppsIt = changes.assignments.constBegin(); ppsIt != changes.assignments.constEnd(); ++ppsIt) {
        QString key = ppsIt.key();

        // lets stuff all change objects in json, will be easier to work with them later
        json[key] = ppsIt.value().toString();
        doc.setObject(json);
    }

    QString payload = doc.toJson();

    if(!json["updateAvailable"].isNull() && json["updateAvailable"].toString() == "0" && json["updateError"].toString().length() > 0) {
        d->updateDetails = json[pps_updateDetailsKey].toString();
        d->updateError = json["updateError"].toString();
        emit updateAvailableChanged(false);
        emit updateDetailsChanged();
        emit updateErrorChanged();
    }

    if(!json["updateAvailable"].isNull() && json["updateAvailable"].toString() == "0" && json["updateError"].isNull()) {
        d->updateDetails = QString();
        d->updateError = QString();
        emit updateAvailableChanged(false);
        emit updateDetailsChanged();
        emit updateErrorChanged();
    }

    if(!json["updateAvailable"].isNull() && json["updateAvailable"].toString() == "1" && !json[pps_updateDetailsKey].isNull()) {
        d->updateDetails = json[pps_updateDetailsKey].toString();
        d->updateError = QString();
        emit updateAvailableChanged(true);
        emit updateDetailsChanged();
        emit updateErrorChanged();
    }
}

void SystemInfo::setAvailable(bool value)
{
    d->m_available = value;
    emit availableChanged(d->m_available);
}

}
