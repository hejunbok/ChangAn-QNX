#include "ManeuversModel.h"

#include "Maneuver_p.h"

namespace QnxCar {

Maneuver::Maneuver()
    : distance(0)
{}

ManeuversModel::Command ManeuversModel::commandFromString(const QString &str, bool *ok)
{
    if (ok)
        *ok = true;
    if (str == QStringLiteral("dt"))
        return ArrivedAtDestination;
    if (str == QStringLiteral("dt-l"))
        return DestinationIsOnTheLeft;
    if (str == QStringLiteral("dt-r"))
        return DestinationIsOnTheRight;
    if (str == QStringLiteral("lht-rx"))
        return FollowRoundaboutOnTheLeft;
    if (str == QStringLiteral("lht-ut"))
        return UTurnOnTheLeft;
    if (str == QStringLiteral("nc"))
        return NoChange;
    if (str == QStringLiteral("rx"))
        return FollowRoundaboutOnTheRight;
    if (str == QStringLiteral("tr-l"))
        return TurnLeft;
    if (str == QStringLiteral("tr-r"))
        return TurnRight;
    if (str == QStringLiteral("ut"))
        return UTurnOnTheRight;
    if (ok)
        *ok = false;
    qWarning("Could not parse navigation command \"%s\"", qPrintable(str));
    return NoChange;
}

QString ManeuversModel::commandToString(Command command)
{
    switch (command) {
    case ArrivedAtDestination:
        return QStringLiteral("dt");
    case DestinationIsOnTheLeft:
        return QStringLiteral("dt-l");
    case DestinationIsOnTheRight:
        return QStringLiteral("dt-r");
    case FollowRoundaboutOnTheLeft:
        return QStringLiteral("lht-rx");
    case UTurnOnTheLeft:
        return QStringLiteral("lht-ut");
    case NoChange:
        return QStringLiteral("nc");
    case FollowRoundaboutOnTheRight:
        return QStringLiteral("rx");
    case TurnLeft:
        return QStringLiteral("tr-l");
    case TurnRight:
        return QStringLiteral("tr-r");
    case UTurnOnTheRight:
        return QStringLiteral("ut");
    }

    Q_ASSERT(!"unhandled command value");
    return QString();
}

class ManeuversModel::Private
{
public:
    QVector<Maneuver> maneuvers;
};

ManeuversModel::ManeuversModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

ManeuversModel::~ManeuversModel()
{
    delete d;
}

int ManeuversModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return d->maneuvers.count();
}

QVariant ManeuversModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    const Maneuver &m = d->maneuvers[index.row()];
    switch (role) {
    case Qt::DisplayRole:
    case StreetRole:
        return m.street;
    case CommandRole:
        return m.command;
    case DistanceRole:
        return m.distance;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> ManeuversModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names.insert(StreetRole, "street");
    names.insert(CommandRole, "command");
    names.insert(DistanceRole, "distance");
    return names;
}

void ManeuversModel::setManeuvers(const QVector<Maneuver> &maneuvers)
{
    if (d->maneuvers.size() == maneuvers.size()) {
        d->maneuvers = maneuvers;
    } else if (d->maneuvers.size() < maneuvers.size()) {
        beginInsertRows(QModelIndex(), d->maneuvers.size(), maneuvers.size()-1);
        d->maneuvers = maneuvers;
        endInsertRows();
        emit rowCountChanged(rowCount());
    } else {
        Q_ASSERT(d->maneuvers.size() > maneuvers.size());
        beginRemoveRows(QModelIndex(), maneuvers.size(), d->maneuvers.size()-1);
        d->maneuvers = maneuvers;
        endRemoveRows();
        emit rowCountChanged(rowCount());
    }

    emit dataChanged(index(0, 0), index(d->maneuvers.size() - 1, 0));
}

}
