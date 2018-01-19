#include "Navigation.h"
#include "Navigation_p.h"
#include "Location.h"
#include "Maneuver_p.h"

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <qpps/changeset.h>

namespace QnxCar {

static void insertIfNotEmpty(QJsonObject &object, const QString &key, const QString &value)
{
    if (!value.isEmpty())
        object.insert(key, value);
}

static QJsonObject locationToJson(const Location &l)
{
    QJsonObject map;
    insertIfNotEmpty(map, QStringLiteral("city"), l.city());
    insertIfNotEmpty(map, QStringLiteral("country"), l.country());
    if (l.distance() > 0)
        map.insert(QStringLiteral("distance"), l.distance());
    if (l.id() != -1)
        map.insert(QStringLiteral("id"), l.id());
    if (!qFuzzyCompare(l.latitude(), 0))
        map.insert(QStringLiteral("latitude"), l.latitude());
    if (!qFuzzyCompare(l.longitude(), 0))
        map.insert(QStringLiteral("longitude"), l.longitude());
    insertIfNotEmpty(map, QStringLiteral("name"), l.name());
    insertIfNotEmpty(map, QStringLiteral("number"), l.streetNumber());
    insertIfNotEmpty(map, QStringLiteral("postalCode"), l.postalCode());
    insertIfNotEmpty(map, QStringLiteral("province"), l.province());
    insertIfNotEmpty(map, QStringLiteral("street"), l.street());
    insertIfNotEmpty(map, QStringLiteral("type"), l.type());
    return map;
}

static Location locationFromJson(const QJsonDocument &json)
{
    const QVariantMap map = json.toVariant().toMap();
    Location l;
    l.setCity(map.value(QStringLiteral("city")).toString());
    l.setCountry(map.value(QStringLiteral("country")).toString());
    l.setDistance(map.value(QStringLiteral("distance")).toInt());
    l.setId(map.value(QStringLiteral("id"), -1).toInt());
    l.setLatitude(map.value(QStringLiteral("latitude")).toDouble());
    l.setLongitude(map.value(QStringLiteral("longitude")).toDouble());
    l.setName(map.value(QStringLiteral("name")).toString());
    l.setStreetNumber(map.value(QStringLiteral("number")).toString());
    l.setPostalCode(map.value(QStringLiteral("postalCode")).toString());
    l.setProvince(map.value(QStringLiteral("province")).toString());
    l.setStreet(map.value(QStringLiteral("street")).toString());
    l.setType(map.value(QStringLiteral("type")).toString());
    return l;
}


Navigation::Private::Private(Navigation *qq)
    : q(qq)
    , ppsControl(QStringLiteral("/pps/qnxcar/navigation/control"), QPps::Object::PublishAndSubscribeMode)
    , ppsStatus(QStringLiteral("/pps/qnxcar/navigation/status"), QPps::Object::SubscribeMode)
{
    if (!ppsControl.isValid()) {
        qWarning() << Q_FUNC_INFO << "Could not open PPS control object:" << ppsStatus.errorString();
    }
    if (ppsStatus.isValid()) {
        connect(&ppsStatus, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(statusChanged(QString,QPps::Variant)));
        ppsStatus.setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS status object:" << ppsStatus.errorString();
    }
}

void Navigation::Private::searchFinished(QnxCar::ControlCommand *command)
{
    if (command->hasError()) {
        qCritical() << Q_FUNC_INFO << "Could not search" << command->errorString();
    }
    emit q->searchResultsReceived();
}

void Navigation::Private::getPointsOfInterestFinished(QnxCar::ControlCommand *command)
{
    if (command->hasError()) {
        qCritical() << Q_FUNC_INFO << "Could not get POIs" << command->errorString();
    }
    emit q->pointsOfInterestResultsReceived();
}

void Navigation::Private::statusChanged(const QString &name, const QPps::Variant &value)
{
    if (name == QStringLiteral("navigating")) {
        emit q->navigatingChanged(value.toBool());
    } else if (name == QStringLiteral("total_distance_remaining")) {
        emit q->totalDistanceRemainingChanged(value.toInt());
    } else if (name == QStringLiteral("total_time_remaining")) {
        emit q->totalTimeRemainingChanged(value.toInt());
    } else if (name == QStringLiteral("maneuvers")) {
        const QVariantList vs = value.toJson().toVariant().toList();
        QVector<Maneuver> maneuvers;
        maneuvers.reserve(vs.size());
        Q_FOREACH(const QVariant &v, vs) {
            const QVariantMap map = v.toMap();
            Maneuver m;
            m.street = map.value(QStringLiteral("street")).toString();
            m.command = ManeuversModel::commandFromString(map.value(QStringLiteral("command")).toString());
            bool ok;
            m.distance = map.value(QStringLiteral("distance")).toInt(&ok);
            if (!ok)
                qWarning() << Q_FUNC_INFO << "Could not parse distance (expected int) from" << map.value(QStringLiteral("distance")).toString();
            maneuvers.append(m);
        }
        maneuversModel.setManeuvers(maneuvers);
    } else if (name == QStringLiteral("destination")) {
        const Location l = locationFromJson(value.toJson());
        destination = l;
        emit q->destinationChanged(destination);
    } else {

    }
}

Navigation::Navigation(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

bool Navigation::navigating() const
{
    return d->ppsStatus.attribute(QStringLiteral("navigating")).toBool();
}

int Navigation::totalDistanceRemaining() const
{
    return d->ppsStatus.attribute(QStringLiteral("total_distance_remaining")).toInt();
}

int Navigation::totalTimeRemaining() const
{
    return d->ppsStatus.attribute(QStringLiteral("total_time_remaining")).toInt();
}

Location Navigation::destination() const
{
    return d->destination;
}

ManeuversModel *Navigation::maneuversModel() const
{
    return &d->maneuversModel;
}

//workaround: eb-navigation crashes when sending '{ "location"...' instead of '{"location":...'
//so remove that manually. All other whitespace seems ok.
static QPps::Variant fixUpJson(const QPps::Variant &v)
{
    QByteArray plain = v.value();
    const int idx = plain.indexOf("location");
    Q_ASSERT(idx > 0);
    QByteArray left = plain.left(idx);
    left.replace(" ", "");
    const QByteArray right = plain.mid(idx);
    return QPps::Variant(left + right, "json");
}

QnxCar::ControlCommand *Navigation::navigateTo(const Location &location)
{
    Q_ASSERT(location.isValid());
    QJsonObject object;
    object.insert(QStringLiteral("location"), locationToJson(location));
    const QPps::Variant dat = fixUpJson(QJsonDocument(object));

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    command->setMessage(QStringLiteral("navigateTo"));
    command->setDat(dat);
    command->start();
    return command;
}

QnxCar::ControlCommand *Navigation::cancelNavigation()
{
    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    command->setMessage(QStringLiteral("cancelNavigation"));
    command->start();
    return command;
}

QnxCar::ControlCommand *Navigation::panMap(int deltaX, int deltaY)
{
    QJsonObject dat;
    dat.insert(QStringLiteral("deltaX"), deltaX);
    dat.insert(QStringLiteral("deltaY"), deltaY);

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    command->setMessage(QStringLiteral("panMap"));
    command->setDat(QJsonDocument(dat));
    command->start();
    return command;
}

QnxCar::ControlCommand *Navigation::zoomMap(qreal scale)
{
    QJsonObject dat;
    dat.insert(QStringLiteral("scale"), scale);

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    command->setMessage(QStringLiteral("zoomMap"));
    command->setDat(QJsonDocument(dat));
    command->start();
    return command;
}

QnxCar::ControlCommand *Navigation::getPointsOfInterest(int categoryId)
{
    QJsonObject dat;
    dat.insert(QStringLiteral("category"), categoryId);

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    connect(command, SIGNAL(finished(QnxCar::ControlCommand*)), d, SLOT(getPointsOfInterestFinished(QnxCar::ControlCommand*)));
    command->setMessage(QStringLiteral("getPOIs"));
    command->setDat(QJsonDocument(dat));
    command->start();
    return command;
}

QnxCar::ControlCommand *Navigation::searchPointsOfInterest(const QString &name)
{
    QJsonObject dat;
    dat.insert(QStringLiteral("name"), name);

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    connect(command, SIGNAL(finished(QnxCar::ControlCommand*)), d, SLOT(getPointsOfInterestFinished(QnxCar::ControlCommand*)));
    command->setMessage(QStringLiteral("getPOIsByName"));
    command->setDat(QJsonDocument(dat));
    command->start();
    return command;
}

QnxCar::ControlCommand *Navigation::search(const QString &country, const QString &city, const QString &street, const QString &number)
{
    QJsonObject  dat;
    if (!country.isEmpty())
        dat.insert(QStringLiteral("country"), country);
    if (!city.isEmpty())
        dat.insert(QStringLiteral("city"), city);
    if (!street.isEmpty())
        dat.insert(QStringLiteral("street"), street);
    if (!number.isEmpty())
        dat.insert(QStringLiteral("number"), number);

    QnxCar::ControlCommand* command = new QnxCar::ControlCommand(&d->ppsControl, this);
    connect(command, SIGNAL(finished(QnxCar::ControlCommand*)), d, SLOT(searchFinished(QnxCar::ControlCommand*)));
    command->setMessage(QStringLiteral("search"));
    command->setDat(QJsonDocument(dat));
    command->start();
    return command;
}

}
