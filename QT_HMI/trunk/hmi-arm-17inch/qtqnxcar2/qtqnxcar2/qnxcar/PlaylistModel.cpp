#include "PlaylistModel.h"

#include "PlaylistModel_p.h"

#include "services/AudioPlayer.h"
#include <QSqlQuery>

#include <iostream>

namespace QnxCar {

static const QString SQL_RECORD_LIMIT = QStringLiteral(" LIMIT 250");

static const QString SQL_SELECT_SONGS_TPL = QLatin1Literal(
    "SELECT "
    "coalesce(nullif(trim(m.title),''), f.filename) as title, "
    "m.duration as duration, "
    "f.fid as fid, "
    "coalesce(nullif(trim(al.album),''), 'Unknown Album') as album, "
    "coalesce(nullif(trim(ar.artist),''), 'Unknown Artist') as artist, "
    "aw.artwork_url as artwork "
    "FROM files f "
    "INNER JOIN audio_metadata m ON f.fid = m.fid "
    "LEFT JOIN albums al ON m.album_id = al.album_id "
    "LEFT JOIN artists ar ON m.artist_id = ar.artist_id "
    "LEFT JOIN artworks aw ON m.album_id = aw.album_id AND aw.type = %1 ");

/// arg1: playername
static const QString SQL_SELECT_TRACKSESSION_SONGS = SQL_SELECT_SONGS_TPL.arg('1') + QLatin1Literal(
    "INNER JOIN player_%1 p ON f.fid = p.fid "
    "ORDER BY p.trkid") +
    SQL_RECORD_LIMIT;

PlaylistModel::PlaylistModel(const QString &playerName, QObject* parent)
    : SqlQueryModel(new Private(playerName), parent)
{
    Q_ASSERT(!D_PTR->playerName.isEmpty());
}

QList<QByteArray> PlaylistModel::columnNames() const
{
    return QList<QByteArray>() << "title" << "duration" << "fid" << "album" << "artist" << "artwork";
}

void PlaylistModel::runQuery()
{
    QSqlQuery query(database());
    query.prepare(SQL_SELECT_TRACKSESSION_SONGS.arg(D_PTR->playerName));
    query.exec();
    setQuery(query);

//    resortData();
}

QString PlaylistModel::titleAt(int index) const
{
    return data(this->index(index, 0), TitleRole).toString().toUtf8();
}

int PlaylistModel::durationAt(int index) const
{
    return data(this->index(index, 0), DurationRole).toInt();
}

int PlaylistModel::fileIdAt(int index) const
{
    return data(this->index(index, 0), FileIdRole).toInt();
}

QString PlaylistModel::albumAt(int index) const
{
    return data(this->index(index, 0), AlbumRole).toString();
}

QString PlaylistModel::artistAt(int index) const
{
    return data(this->index(index, 0), ArtistRole).toString();
}

QString PlaylistModel::artworkAt(int index) const
{
    return data(this->index(index, 0), ArtworkRole).toString();
}

}
