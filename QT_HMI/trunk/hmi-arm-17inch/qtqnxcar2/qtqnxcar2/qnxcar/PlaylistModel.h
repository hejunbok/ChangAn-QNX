#ifndef QTQNXCAR2_PLAYLISTMODEL_H
#define QTQNXCAR2_PLAYLISTMODEL_H

#include "qtqnxcar2_export.h"

#include <qnxcar/SqlQueryModel.h>

namespace QnxCar {

/**
 * Model for listing available playlists from a media player source database
 */
class QTQNXCAR2_EXPORT PlaylistModel : public SqlQueryModel
{
    Q_OBJECT

public:
    /// Item roles
    enum Role {
        /// Song title (QString), role name @c "title"
        TitleRole = Qt::UserRole + 1,
        /// Song duration in ms (int), role name @c "duration"
        DurationRole,
        /// File id (int), role name @c "fid"
        FileIdRole,
        /// Album role (QString), role name @c "album"
        AlbumRole,
        /// Artist role (QString), role name @c "artist"
        ArtistRole,
        /// Artwork (original icon) (QString), role name @c "artwork"
        ArtworkRole
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param playerName name of the player, e.g. @c "mpaudio"
     * @param parent the QObject parent
     */
    explicit PlaylistModel(const QString &playerName, QObject *parent = 0);

    // Accessors for QML, no way around that code duplication because of
    // https://bugreports.qt-project.org/browse/QTBUG-16041

    /**
     * Returns the title of the playlist entry at row @p index
     *
     * @param index the row index in the playlist
     * @return the title of the entry at @p index or QString()
     * @sa #TitleRole
     */
    Q_INVOKABLE QString titleAt(int index) const;

    /**
     * Returns the duration of the playlist entry at row @p index
     *
     * @param index the row index in the playlist
     * @return the duration of the entry at @p index or 0
     * @sa #DurationRole
     */
    Q_INVOKABLE int durationAt(int index) const;

    /**
     * Returns the file identifier of the playlist entry at row @p index
     *
     * @param index the row index in the playlist
     * @return the file identifier of the entry at @p index or 0
     * @sa #FileIdRole
     */
    Q_INVOKABLE int fileIdAt(int index) const;

    /**
     * Returns the album of the playlist entry at row @p index
     *
     * @param index the row index in the playlist
     * @return the album of the entry at @p index or QString()
     * @sa #AlbumRole
     */
    Q_INVOKABLE QString albumAt(int index) const;

    /**
     * Returns the artist of the playlist entry at row @p index
     *
     * @param index the row index in the playlist
     * @return the artist of the entry at @p index or QString()
     * @sa #ArtistRole
     */
    Q_INVOKABLE QString artistAt(int index) const;

    /**
     * Returns the artwork/icon of the playlist entry at row @p index
     *
     * @param index the row index in the playlist
     * @return the artwork/icon of the entry at @p index or QString()
     * @sa #ArtworkRole
     */
    Q_INVOKABLE QString artworkAt(int index) const;


protected:
    void runQuery() Q_DECL_OVERRIDE;
//    void resortData();
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;

private:
    class Private;
};

}

#endif
