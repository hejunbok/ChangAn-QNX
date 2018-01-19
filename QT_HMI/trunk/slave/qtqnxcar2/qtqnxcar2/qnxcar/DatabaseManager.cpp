#include "DatabaseManager.h"

#include "util/StandardDirs.h"
#include "util/Util.h"

#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QSqlDatabase>

namespace QnxCar {

static const QString DEFAULT_DATABASE_TYPE = QStringLiteral("QSQLITE");

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

QString DatabaseManager::databaseDirectory()
{
#ifdef CAR2_HOST_BUILD
    return QString(QStringLiteral("%1/sampledata/databases/")).arg(StandardDirs::assetsDirectory());
#else
    return QStringLiteral("/db");
#endif
}

QString DatabaseManager::databasePath(const QString &databaseId)
{
    if (databaseId.isEmpty())
        return QString();

    if (QFileInfo(databaseId).isAbsolute())
        return databaseId;
    else
        return QString(QStringLiteral("%1/%2").arg(databaseDirectory()).arg(databaseId));
}

static QSqlDatabase createOrGetDatabase(const QString& filePath)
{
    // default to one connection per file at the moment
    if (QSqlDatabase::contains(filePath))
        return QSqlDatabase::database(filePath);

    QSqlDatabase db = QSqlDatabase::addDatabase(DEFAULT_DATABASE_TYPE, filePath);
    db.setDatabaseName(filePath);
    return db;
}

QSqlDatabase DatabaseManager::database(const QString& fileName)
{
    return createOrGetDatabase(databasePath(fileName));
}

QSqlDatabase DatabaseManager::forQdbPath(const QString &path)
{
    const QFileInfo info(path);
    const QString fileName = QStringLiteral("%1/%2.db").arg(databaseDirectory(), info.fileName());
    return createOrGetDatabase(fileName);
}

}
