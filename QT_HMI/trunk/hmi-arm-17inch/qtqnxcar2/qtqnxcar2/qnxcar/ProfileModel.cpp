#include "ProfileModel.h"
#include "ProfileData_p.h"

#include "util/Util.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>

using namespace QnxCar;

/// Unfortunately we have to maintain another mapping from column name to column
enum Column {
    IdColumn = 0,
    FullNameColumn,
    DeviceIdColumn,
    ThemeColumn,
    AvatarColumn,
    AvatarFilePathColumn
};


QList<QVariant> ProfileData::values() const
{
    QList<QVariant> ret;
    if (!isValid())
        return ret;

    ret += fullName;
    ret += deviceId;
    ret += theme;
    ret += avatar;
    ret += avatarFilePath;
    return ret;
}

ProfileData ProfileData::fromSqlRecord(const QSqlRecord &record)
{
    Q_ASSERT(!record.isEmpty());

    ProfileData data;
    data.fullName = record.field(FullNameColumn).value().toString();
    data.deviceId = record.field(DeviceIdColumn).value().toString();
    data.theme = record.field(ThemeColumn).value().toString();
    data.avatar = record.field(AvatarColumn).value().toString();
    data.avatarFilePath = record.field(AvatarFilePathColumn).value().toString();
    return data;
}

ProfileModel::ProfileModel(const QSqlDatabase &db_, QObject *parent)
    : SqlQueryModel(parent)
{
    QSqlDatabase db(db_);
    if (!db.isOpen() && !db.open())
        qWarning() << "Failed to open database:" << db << "- error:" << db.lastError();
    setDatabase(db);
    update();
}

ProfileModel::~ProfileModel()
{
}

QList<QByteArray> ProfileModel::columnNames() const
{
    return QList<QByteArray>()
        << "id"
        << "full_name"
        << "device_id"
        << "theme"
        << "avatar"
        << "avatar_file_path";
}

static QString joinWithCommas(const QList<QByteArray> &list)
{
    QString ret;
    foreach (const QByteArray &ba, list) {
        ret += QString::fromLatin1(ba);
        ret += QStringLiteral(", ");
    }
    ret.chop(2);
    return ret;
}

int ProfileModel::addProfile(const ProfileData &data)
{
    // do not generate SQL for inserting the ID column; the database creates the primary key.
    QList<QByteArray> dataColumns = columnNames().mid(1);

    QList<QByteArray> dcPlaceholders;
    foreach (const QByteArray &colName, dataColumns)
        dcPlaceholders += QByteArrayLiteral(":") + colName;

    QString cmd = QStringLiteral("INSERT INTO profiles (%1) VALUES (%2)")
                  .arg(joinWithCommas(dataColumns), joinWithCommas(dcPlaceholders));

    QSqlQuery insertQuery(database());
    insertQuery.prepare(cmd);

    QList<QVariant> fields = data.values();
    for (int i = 0; i < fields.count(); i++)
        insertQuery.bindValue(i, fields[i]);

    bool ok = insertQuery.exec();
    Q_ASSERT(ok);

    // reload...
    update();

    const int newId = record(rowCount() - 1).field(0).value().toInt(&ok);
    Q_ASSERT(ok);
    return newId;
}

int ProfileModel::addProfile(const QString &fullName, const QString &avatar,
                             const QString &avatarFilePath, const QString &theme)
{
    ProfileData data;
    data.fullName = fullName;
    data.avatar = avatar;
    data.avatarFilePath = avatarFilePath;
    data.theme = theme;
    return addProfile(data);
}

int ProfileModel::addProfile()
{
    return addProfile(tr("Unnamed"), QStringLiteral("male1"),
                      QStringLiteral(""), QStringLiteral("default"));
}

bool ProfileModel::removeProfile(int id)
{
    const int row = rowForId(id);
    if (row == -1) {
        return false;
    }

    QString cmd = QStringLiteral("DELETE FROM profiles WHERE id=%1").arg(id);
    QSqlQuery deleteQuery(cmd, database());
    const bool ok = !deleteQuery.lastError().isValid();
    Q_ASSERT(ok);
    Q_UNUSED(ok) //release mode

    car2_debug() << id;

    update();

    return true;
}

bool ProfileModel::updateProfile(int id, const ProfileData& data)
{
    const int row = rowForId(id);
    if (row == -1) {
        car2_debug() << "No such ID:" << id;
        return false;
    }

    QString updates;
    QList<QByteArray> dataColumns = columnNames().mid(1);
    foreach (const QByteArray &dataColumn, dataColumns) {
        updates += QString::fromLatin1(dataColumn);
        updates += QStringLiteral("=:");
        updates += QString::fromLatin1(dataColumn);
        updates += QStringLiteral(", ");
    }
    updates.chop(2);

    QString cmd = QStringLiteral("UPDATE profiles SET %1 WHERE id=:id").arg(updates);
    QSqlQuery updateQuery(database());
    updateQuery.prepare(cmd);

    QList<QVariant> fields = data.values();
    Q_ASSERT(!fields.isEmpty());
    fields += id;
    for (int i = 0; i < fields.count(); i++)
        updateQuery.bindValue(i, fields[i]);

    const bool ok = updateQuery.exec();
    Q_ASSERT(ok);
    Q_UNUSED(ok) //release mode

    update();

    return true;
}

ProfileData ProfileModel::profile(int id) const
{
    const int row = rowForId(id);
    return dataAt(row);
}

QList<int> ProfileModel::profileIds() const
{
    QList<int> ids;
    for (int row = 0; row < rowCount(); ++row) {
        ids << record(row).field(IdColumn).value().toInt();
    }
    return ids;
}

void ProfileModel::runQuery()
{
    QString cmd = QStringLiteral("SELECT %1 from profiles ORDER BY id").arg(joinWithCommas(columnNames()));
    QSqlQuery query(cmd, database());

    bool ok = !query.lastError().isValid();
    if (!ok) {
        qWarning() << Q_FUNC_INFO << "Failed update - error:"
                   << query.lastError() << query.executedQuery();
    }

    setQuery(query);
}

int ProfileModel::idAt(int row) const
{
    QSqlRecord rec = record(row);
    if (rec.isEmpty()) {
        return -1;
    }
    return rec.field(IdColumn).value().toInt();
}

ProfileData ProfileModel::dataAt(int row) const
{
    if (row < 0 || row >= rowCount()) {
        Q_ASSERT(false);
        return ProfileData();
    }
    return ProfileData::fromSqlRecord(record(row));
}

int ProfileModel::rowForId(int id) const
{
    return profileIds().indexOf(id);
}
