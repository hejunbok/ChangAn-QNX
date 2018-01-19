#include "SettingsModels.h"

#include "util/Util.h"

#include "qnxcar/User.h"

#include "services/BluetoothDevicesModel.h"

#include <qpps/object.h>

#include <QDebug>
#include <QJsonDocument>
#include <QStringList>
#include <QVector>

#include "iostream"

namespace QnxCar {

struct ThemeListModel::ThemeData
{
    QString title;
    QString themePackageName;
};

class ThemeListModel::Private
{
public:
    QPps::Object *ppsObject;
    QMap<QString, ThemeData> themeCache;
};

ThemeListModel::ThemeListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    d->ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/themes"),
                                    QPps::Object::SubscribeMode, true, this);

    if (d->ppsObject->isValid()) {
        connect(d->ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        d->ppsObject->setAttributeCacheEnabled(true);
        reloadData();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << d->ppsObject->errorString();
    }
}

ThemeListModel::~ThemeListModel()
{
    delete d;
}

void ThemeListModel::reloadData()
{
    beginResetModel();

    // clear
    d->themeCache.clear();

    // reload
    foreach (const QString& themeId, d->ppsObject->attributeNames()) {
        const QPps::Variant attribute = d->ppsObject->attribute(themeId);
        if (!attribute.isValid())
            continue;

        const ThemeData data = dataFromAttributeValue(attribute);
        d->themeCache.insert(themeId, data);
    }

    endResetModel();
}

// TODO: Improve this logic?
ThemeListModel::ThemeData ThemeListModel::dataFromAttributeValue(const QPps::Variant& attribute) const
{
    ThemeData data;
    const QVariantMap map = attribute.toJson().toVariant().toMap();
    if (map.isEmpty())
        return data;

    data.title = map.value(QStringLiteral("title")).toString();
    data.themePackageName = map.value(QStringLiteral("themePackageName")).toString();
    return data;
}

int ThemeListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->themeCache.size();
}

QVariant ThemeListModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= d->themeCache.size())
        return QVariant();

    if (role == Qt::DisplayRole || role == ThemeListModel::TitleRole) {
        return dataAt(row).title;
    } else if (role == ThemeListModel::IdRole) {
        return idAt(row);
    } else if (role == ThemeListModel::ThemePackageNameRole) {
        return dataAt(row).themePackageName;
    }
    return QVariant();
}

QString ThemeListModel::idAt(int row) const
{
    if (row < 0 || row >= d->themeCache.size())
        return QString();
    return (d->themeCache.begin() + row).key();
}

ThemeListModel::ThemeData ThemeListModel::dataAt(int row) const
{
    if (row < 0 || row >= d->themeCache.size())
        return ThemeData();
    return (d->themeCache.begin() + row).value();
}

int ThemeListModel::rowForThemeId(const QString& id) const
{
    QMap<QString, ThemeData>::const_iterator it = d->themeCache.find(id);
    if (it == d->themeCache.end())
        return -1;

    return std::distance(d->themeCache.constBegin(), it);
}

QHash<int, QByteArray> ThemeListModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[IdRole] = "id";
    names[TitleRole] = "title";
    names[ThemePackageNameRole] = "themePackageName";
    return names;
}

void ThemeListModel::ppsAttributeChanged(const QString& name, const QPps::Variant& attribute)
{
    Q_UNUSED(name);
    Q_UNUSED(attribute);

    // themes listing changed, completely refetch data for now
    reloadData();
}

struct AvatarListModel::AvatarImageData
{
    explicit AvatarImageData(const QString& _name)
        : name(_name) {}
    AvatarImageData() {}

    QString name;
};

class AvatarListModel::Private
{
public:
    QMap<QString, AvatarImageData> avatarImageData;
};

AvatarListModel::AvatarListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    loadData();
}

AvatarListModel::~AvatarListModel()
{
    delete d;
}

void AvatarListModel::loadData()
{
    d->avatarImageData[QStringLiteral("male1")] = AvatarImageData(tr("Male 1"));
    d->avatarImageData[QStringLiteral("male2")] = AvatarImageData(tr("Male 2"));
    d->avatarImageData[QStringLiteral("female1")] = AvatarImageData(tr("Female 1"));
    d->avatarImageData[QStringLiteral("female2")] = AvatarImageData(tr("Female 2"));
}

QHash<int, QByteArray> AvatarListModel::roleNames() const
{
  QHash<int, QByteArray> names = QAbstractListModel::roleNames();
  names[IdRole] = "id";
  names[NameRole] = "name";
  return names;
}

int AvatarListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->avatarImageData.size();
}

QVariant AvatarListModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= d->avatarImageData.size())
        return QVariant();

    if (role == Qt::DisplayRole || role == AvatarListModel::NameRole) {
        return dataAt(row).name;
    } else if (role == AvatarListModel::IdRole) {
        return idAt(row);
    }
    return QVariant();
}

int AvatarListModel::rowForAvatarId(const QString& id) const
{
    QMap<QString, AvatarImageData>::const_iterator it = d->avatarImageData.find(id);
    if (it == d->avatarImageData.end())
        return -1;

    return std::distance(d->avatarImageData.constBegin(), it);
}

QString AvatarListModel::idAt(int row) const
{
    if (row < 0 || row >= d->avatarImageData.size())
        return QString();
    return (d->avatarImageData.begin() + row).key();
}

AvatarListModel::AvatarImageData AvatarListModel::dataAt(int row) const
{
    if (row < 0 || row >= d->avatarImageData.size())
        return AvatarImageData();
    return (d->avatarImageData.begin() + row).value();
}

class  SettingsModels::Private
{
public:
    Private(SettingsModels *q)
        : avatarListModel(new AvatarListModel(q))
        , themeListModel(new ThemeListModel(q))
        , bluetoothDevicesModel(new BluetoothDevicesModel(BluetoothDevicesModel::PairedDevices, q->parent()))
    {}

    AvatarListModel* avatarListModel;
    ThemeListModel* themeListModel;
    BluetoothDevicesModel* bluetoothDevicesModel;
};

SettingsModels::SettingsModels(QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
}

SettingsModels::~SettingsModels()
{
    delete d;
}

AvatarListModel *SettingsModels::avatarListModel() const
{
    return d->avatarListModel;
}

ThemeListModel *SettingsModels::themeListModel() const
{
    return d->themeListModel;
}

BluetoothDevicesModel *SettingsModels::bluetoothDevicesModel() const
{
    return d->bluetoothDevicesModel;
}

}
