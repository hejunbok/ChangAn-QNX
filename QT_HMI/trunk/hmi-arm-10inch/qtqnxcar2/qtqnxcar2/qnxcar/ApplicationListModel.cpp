#include "ApplicationListModel.h"

#include <QDebug>

namespace QnxCar {

class ApplicationData::Private : public QSharedData
{
public:
    QString id;
    QString name;
    QString group;
    QString uri;
    QString iconPath;

    inline bool isValid() const
    {
        return !id.isEmpty();
    }
};

ApplicationData::ApplicationData()
    : d(new Private)
{
}

ApplicationData::ApplicationData(const ApplicationData &other)
    : d(other.d)
{
}

ApplicationData::~ApplicationData()
{
}

ApplicationData &ApplicationData::operator =(const ApplicationData &other)
{
    if (&other == this)
        return *this;

    d = other.d;
    return *this;
}

QString ApplicationData::id() const
{
    return d->id;
}

void ApplicationData::setId(const QString &id)
{
    d->id = id;
}

QString ApplicationData::name() const
{
    return d->name;
}

void ApplicationData::setName(const QString &name)
{
    d->name = name;
}

QString ApplicationData::group() const
{
    return d->group;
}

void ApplicationData::setGroup(const QString &group)
{
    d->group = group;
}

QString ApplicationData::iconPath() const
{
    return d->iconPath;
}

void ApplicationData::setIconPath(const QString &iconPath)
{
    d->iconPath = iconPath;
}

QString ApplicationData::uri() const
{
    return d->uri;
}

void ApplicationData::setUri(const QString &uri)
{
    d->uri = uri;
}

bool ApplicationData::isValid() const
{
    return d->isValid();
}

QDebug operator<<(QDebug dbg, const ApplicationData& data)
{
    return dbg << data.id();
}

class ApplicationListModel::Private
{
public:
    QVector<ApplicationData> data;
};

ApplicationListModel::ApplicationListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

ApplicationListModel::~ApplicationListModel()
{
    delete d;
}

int ApplicationListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->data.size();
}

QVariant ApplicationListModel::data(const QModelIndex &index, int role) const
{
    const ApplicationData& appData = d->data[index.row()];

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return appData.name();
    case AppIdRole:
        return appData.id();
    case GroupRole:
        return appData.group();
    case UriRole:
        return appData.uri();
    case IconPathRole:
        return appData.iconPath();
    default:
        break;
    }

    return QVariant();
}

QHash<int,QByteArray> ApplicationListModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();

    names.insert(AppIdRole, "appId");
    names.insert(NameRole, "name");
    names.insert(GroupRole, "group");
    names.insert(UriRole, "uri");
    names.insert(IconPathRole, "iconPath");

    return names;
}

void ApplicationListModel::setApplications(const QVector<ApplicationData> &applications)
{
    beginResetModel();
    d->data = applications;
    endResetModel();
}

QVector<ApplicationData> ApplicationListModel::applications() const
{
    return d->data;
}

ApplicationData ApplicationListModel::findByName(const QString &appName) const
{
    Q_FOREACH (const ApplicationData &data, d->data) {
        if (data.name() == appName) {
            return data;
        }
    }
    return ApplicationData();
}

ApplicationData ApplicationListModel::findById(const QString &appId) const
{
    Q_FOREACH (const ApplicationData &data, d->data) {
        if (data.id() == appId) {
            return data;
        }
    }
    return ApplicationData();
}

void ApplicationListModel::addApplication(const ApplicationData &application)
{
    for (int i = 0; i < d->data.size(); ++i) {
        if (d->data[i].id() == application.id()) {
            d->data[i] = application;
            emit dataChanged(index(i, 0), index(i, 0));
            return;
        }
    }

    beginInsertRows(QModelIndex(), d->data.size(), d->data.size());
    d->data.append(application);
    endInsertRows();
}

void ApplicationListModel::removeApplication(const QString &appId)
{
    for (int i = 0; i < d->data.size(); ++i) {
        if (d->data[i].id() == appId) {
            beginRemoveRows(QModelIndex(), i, i);
            d->data.remove(i);
            endRemoveRows();
            return;
        }
    }
}

}
