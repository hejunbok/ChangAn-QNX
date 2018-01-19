#include "VideosModel.h"

#include <QSqlQuery>
#include "iostream"

namespace QnxCar {

static const QString SQL_RECORD_LIMIT = QStringLiteral(" LIMIT 250");

static const QString SQL_SELECT_VIDEOS = QLatin1Literal(
    "SELECT "
    "f.fid as fid, "
    "m.duration as duration, "
    "coalesce(nullif(trim(m.title),''), f.filename) as title, "
    "fd.basepath as basepath, "
    "f.filename as filename "
    "FROM files f "
    "INNER JOIN video_metadata m ON f.fid = m.fid "
    "LEFT JOIN folders fd ON f.folderid = fd.folderid "
    "ORDER BY UPPER(title)") +
    SQL_RECORD_LIMIT;

VideosModel::VideosModel(QObject *parent)
    : SqlQueryModel(parent)
{
}

QList<QByteArray> VideosModel::columnNames() const
{
    return QList<QByteArray>() << "fid" << "duration" << "title" << "basepath" << "filename" << "path";
}

QVariant VideosModel::data(const QModelIndex &item, int role) const
{
    if (role == PathRole) {
        return data(item, BasePathRole).toString() + data(item, FileNameRole).toString();
    }

    return SqlQueryModel::data(item, role);
}

void VideosModel::runQuery()
{
    setQuery(SQL_SELECT_VIDEOS, database());
}

int VideosModel::fileIdAt(int index) const
{
    return data(this->index(index, 0), FileIdRole).toInt();
}

int VideosModel::durationAt(int index) const
{
    return data(this->index(index, 0), DurationRole).toInt();
}

QString VideosModel::titleAt(int index) const
{
    return data(this->index(index, 0), TitleRole).toString();
}

QString VideosModel::basePathAt(int index) const
{
    return data(this->index(index, 0), BasePathRole).toString();
}

QString VideosModel::fileNameAt(int index) const
{
    return data(this->index(index, 0), FileNameRole).toString();
}

QString VideosModel::pathAt(int index) const
{
    return data(this->index(index, 0), PathRole).toString();
}

int VideosModel::idByPath(const QString &absolutePath) const
{
    for (int i = 0; i < rowCount(); ++i) {
        const QString path = pathAt(i);
        if (absolutePath.endsWith(path))
            return i;
    }
    return -1;
}

}
