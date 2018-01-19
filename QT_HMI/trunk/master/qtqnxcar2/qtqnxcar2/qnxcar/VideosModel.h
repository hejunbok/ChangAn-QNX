#ifndef QTQNXCAR2_VIDEOSMODEL_H
#define QTQNXCAR2_VIDEOSMODEL_H

#include "qtqnxcar2_export.h"

#include <qnxcar/SqlQueryModel.h>

namespace QnxCar {

/**
 * Model for accessing video data of a media player source database
 */
class QTQNXCAR2_EXPORT VideosModel : public SqlQueryModel
{
    Q_OBJECT

public:
    /// Item roles
    enum Role {
        /// File id (int), role name @c "fid"
        FileIdRole = Qt::UserRole + 1,
        /// Video duration in ms (int), role name @c "duration"
        DurationRole,
        /// Video title (QString), role name @c "title"
        TitleRole,
        /// Video base path role (QString), role name @c "basepath"
        BasePathRole,
        /// Video file name (without path), role name @c "filename"
        FileNameRole,
        /// Video path (complete path), role name @c "path"
        PathRole
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit VideosModel(QObject *parent = 0);

    // accessors for QML
    /**
     * Returns the file identifier of the video list entry at row @p index
     *
     * @param index the row index in the video list
     * @return the file identifier of the entry at @p index or 0
     * @sa #FileIdRole
     */
    Q_INVOKABLE int fileIdAt(int index) const;

    /**
     * Returns the duration of the video list entry at row @p index
     *
     * @param index the row index in the video list
     * @return the duration of the entry at @p index or 0
     * @sa #DurationRole
     */
    Q_INVOKABLE int durationAt(int index) const;

    /**
     * Returns the title of the video list entry at row @p index
     *
     * @param index the row index in the video list
     * @return the title of the entry at @p index or QString()
     * @sa #TitleRole
     */
    Q_INVOKABLE QString titleAt(int index) const;

    /**
     * Returns the base path of the video list entry at row @p index
     *
     * @param index the row index in the video list
     * @return the base path of the entry at @p index or QString()
     * @sa #BasePathRole
     */
    Q_INVOKABLE QString basePathAt(int index) const;

    /**
     * Returns the file name of the video list entry at row @p index
     *
     * @param index the row index in the video list
     * @return the file name of the entry at @p index or QString()
     * @sa #FileNameRole
     */
    Q_INVOKABLE QString fileNameAt(int index) const;

    /**
     * Returns the absolute path of the video list entry at row @p index
     *
     * @param index the row index in the video list
     * @return the path of the entry at @p index or QString()
     * @sa #PathRole
     */
    Q_INVOKABLE QString pathAt(int index) const;

    /**
     * Try to look up @p absolutePath in the model
     *
     * Return the index of this item
     *
     * @example
     * Input: /apps/mediasources//dbmme//videos/qnx_jeep_video_june_2012_1280x720.mp4
     * Base path + title at index 0: /videos/qnx_jeep_video_june_2012_1280x720.mp4
     * => Return 0
     */
    Q_INVOKABLE int idByPath(const QString &absolutePath) const;

    QVariant data(const QModelIndex &item, int role) const;

protected:
    void runQuery();
    QList<QByteArray> columnNames() const;
};

}

#endif
