#include <QtTest/QtTest>

#include "qnxcar/AlbumsModel.h"
#include "qnxcar/ArtistsModel.h"
#include "qnxcar/GenresModel.h"
#include "qnxcar/SearchModel.h"
#include "qnxcar/SongsModel.h"
#include "qnxcar/SourcesModel.h"
#include "qnxcar/PlaylistModel.h"
#include "qnxcar/VideosModel.h"
#include "qnxcar/DatabaseManager.h"
#include "simulator/Simulator.h"

#include <qpps/simulator.h>

#include <QSqlRecord>
#include <QString>

using QnxCar::AlbumsModel;
using QnxCar::ArtistsModel;
using QnxCar::GenresModel;
using QnxCar::SearchModel;
using QnxCar::SongsModel;
using QnxCar::SourcesModel;
using QnxCar::PlaylistModel;
using QnxCar::VideosModel;
using QnxCar::DatabaseManager;

static const QString AUDIOPLAYER_PLAYERNAME = QStringLiteral("mpaudio");

class tst_MediaPlayerModels : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAlbumsModel();
    void testArtistsModel();
    void testGenresModel();
    void testSearchModel();
    void testPlaylistModel();
    void testSongsModel_Basic();
    void testSongsModel_Filtering();
    void testVideosModel();
    void testSourcesModel();
};

void tst_MediaPlayerModels::testAlbumsModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    AlbumsModel model;
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 16);

    // test filtering by artist
    model.setFilterArtist(2);
    QCOMPARE(model.rowCount(), 1);
    // this should find the album with album_id == 2
    QCOMPARE(model.data(model.index(0, 0), AlbumsModel::AlbumIdRole).toInt(), 2);
    QCOMPARE(model.data(model.index(0, 0), AlbumsModel::AlbumRole).toString(), QStringLiteral("Raising The Roof!"));

    // test resetting the model
    model.clearFilters();
    QCOMPARE(model.rowCount(), 16);
}

void tst_MediaPlayerModels::testArtistsModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    ArtistsModel model;
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 16);

    // test resetting the model
    model.update();
    QCOMPARE(model.rowCount(), 16);
}

void tst_MediaPlayerModels::testSearchModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    SearchModel model;
    model.setDatabase(db);

    // no search results at the beginning, hence the model should be empty
    QCOMPARE(model.rowCount(), 0);

    // query for non-existing data
    model.setSearchString(QStringLiteral("foo"));
    QCOMPARE(model.rowCount(), 0);

    // query for existing artist
    {
        model.setSearchString(QStringLiteral("Tom"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.index(0, 0).data(SearchModel::DbIdRole).toInt(), 2); // artist_id
        QCOMPARE(model.index(0, 0).data(SearchModel::NameRole).toString(), QStringLiteral("tomprincipato"));
        QCOMPARE(model.index(0, 0).data(SearchModel::TypeRole).toString(), QStringLiteral("artist"));
    }

    // query for existing album
    {
        model.setSearchString(QStringLiteral("Raising The"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.index(0, 0).data(SearchModel::DbIdRole).toInt(), 2); // album_id
        QCOMPARE(model.index(0, 0).data(SearchModel::NameRole).toString(), QStringLiteral("Raising The Roof!"));
        QCOMPARE(model.index(0, 0).data(SearchModel::TypeRole).toString(), QStringLiteral("album"));
    }

    // query for existing song
    {
        model.setSearchString(QStringLiteral("Groove"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.index(0, 0).data(SearchModel::DbIdRole).toInt(), 1); // fid
        QCOMPARE(model.index(0, 0).data(SearchModel::NameRole).toString(), QStringLiteral("Bo Bo's Groove"));
        QCOMPARE(model.index(0, 0).data(SearchModel::TypeRole).toString(), QStringLiteral("song"));
    }

    // test QML role names, note: the result list must be non-empty to test this
    QVERIFY(model.rowCount() > 0);
    const QHash<int, QByteArray> roleNames = model.roleNames();
    QCOMPARE(roleNames[SearchModel::DbIdRole], QByteArrayLiteral("db_id"));
    QCOMPARE(roleNames[SearchModel::NameRole], QByteArrayLiteral("name"));
    QCOMPARE(roleNames[SearchModel::TypeRole], QByteArrayLiteral("type"));

    // test clear
    model.clear();
    QCOMPARE(model.rowCount(), 0);
}

void tst_MediaPlayerModels::testSongsModel_Basic()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    SongsModel model;
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 17);

    // test QML role names
    const QHash<int, QByteArray> roleNames = model.roleNames();
    QCOMPARE(roleNames[SongsModel::FileIdRole], QByteArrayLiteral("fid"));
    QCOMPARE(roleNames[SongsModel::TitleRole], QByteArrayLiteral("title"));
    QCOMPARE(roleNames[SongsModel::DurationRole], QByteArrayLiteral("duration"));
    QCOMPARE(roleNames[SongsModel::AlbumRole], QByteArrayLiteral("album"));
    QCOMPARE(roleNames[SongsModel::ArtistRole], QByteArrayLiteral("artist"));
    QCOMPARE(roleNames[SongsModel::ArtworkRole], QByteArrayLiteral("artwork"));

    const QModelIndex index = model.index(0, 0);
    // check if all roles return sane information
    QCOMPARE(index.data(SongsModel::FileIdRole).toInt(), 1);
    QCOMPARE(index.data(SongsModel::TitleRole).toString(), QStringLiteral("Bo Bo's Groove"));
    QCOMPARE(index.data(SongsModel::DurationRole).toInt(), 318411);
    QCOMPARE(index.data(SongsModel::AlbumRole).toString(), QStringLiteral("Raising The Roof!"));
    QCOMPARE(index.data(SongsModel::ArtistRole).toString(), QStringLiteral("tomprincipato"));
    QCOMPARE(index.data(SongsModel::ArtworkRole).toString(), QStringLiteral("file:///apps/mediasources/imagecache//mme/2/thumb"));
    QVERIFY(index.isValid());
}

void tst_MediaPlayerModels::testSongsModel_Filtering()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    SongsModel model;
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 17);

    // test filtering by artist
    {
        model.setFilterArtist(2);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.filterType(), SongsModel::ArtistFilterType);
        QCOMPARE(model.dbId(), 2);

        const QModelIndex index = model.index(0, 0);
        QVERIFY(index.isValid());
        QCOMPARE(index.data(SongsModel::FileIdRole).toInt(), 1);
    }

    // test filtering by album
    {
        model.setFilterAlbum(3);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.filterType(), SongsModel::AlbumFilterType);
        QCOMPARE(model.dbId(), 3);

        const QModelIndex index = model.index(0, 0);
        QVERIFY(index.isValid());
        QCOMPARE(index.data(SongsModel::FileIdRole).toInt(), 2);
    }

    // test filtering by genre
    {
        model.setFilterGenre(2);
        QCOMPARE(model.rowCount(), 5);
        QCOMPARE(model.filterType(), SongsModel::GenreFilterType);
        QCOMPARE(model.dbId(), 2);

        const QModelIndex index = model.index(0, 0);
        QVERIFY(index.isValid());
        QCOMPARE(index.data(SongsModel::FileIdRole).toInt(), 1);
    }

    // test resetting the model
    model.clearFilters();
    QCOMPARE(model.rowCount(), 17);
    QCOMPARE(model.filterType(), SongsModel::NoFilterType);
    QCOMPARE(model.dbId(), -1);

    // test clearing the model
    model.clear();
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.filterType(), SongsModel::NoFilterType);
    QCOMPARE(model.dbId(), -1);
}

void tst_MediaPlayerModels::testGenresModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    GenresModel model;
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 7);

    // test QML role names
    const QHash<int, QByteArray> roleNames = model.roleNames();
    QCOMPARE(roleNames[GenresModel::GenreIdRole], QByteArrayLiteral("genre_id"));
    QCOMPARE(roleNames[GenresModel::GenreRole], QByteArrayLiteral("genre"));
    QCOMPARE(roleNames[GenresModel::ArtworkRole], QByteArrayLiteral("artwork"));

    // test data (unknown genre)
    QCOMPARE(model.index(0, 0).data(GenresModel::GenreIdRole).toInt(), 1);
    QCOMPARE(model.index(0, 0).data(GenresModel::GenreRole).toString(), QStringLiteral("Unknown Genre"));
    // test data (known genre)
    QCOMPARE(model.index(1, 0).data(GenresModel::GenreIdRole).toInt(), 3);
    QCOMPARE(model.index(1, 0).data(GenresModel::GenreRole).toString(), QStringLiteral("electronica"));

    // test resetting the model
    model.update();
    QCOMPARE(model.rowCount(), 7);
}

void tst_MediaPlayerModels::testVideosModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    VideosModel model;
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 1);

    // test QML role names
    const QHash<int, QByteArray> roleNames = model.roleNames();
    QCOMPARE(roleNames[VideosModel::FileIdRole], QByteArrayLiteral("fid"));
    QCOMPARE(roleNames[VideosModel::DurationRole], QByteArrayLiteral("duration"));
    QCOMPARE(roleNames[VideosModel::TitleRole], QByteArrayLiteral("title"));
    QCOMPARE(roleNames[VideosModel::BasePathRole], QByteArrayLiteral("basepath"));

    // test data
    QCOMPARE(model.index(0, 0).data(VideosModel::FileIdRole).toInt(), 18);
    QCOMPARE(model.index(0, 0).data(VideosModel::DurationRole).toInt(), 188813);
    QCOMPARE(model.index(0, 0).data(VideosModel::TitleRole).toString(), QStringLiteral("qnx_jeep_video_june_2012_1280x720.mp4"));
    QCOMPARE(model.index(0, 0).data(VideosModel::BasePathRole).toString(), QStringLiteral("/videos/"));
    QCOMPARE(model.index(0, 0).data(VideosModel::FileNameRole).toString(), QStringLiteral("qnx_jeep_video_june_2012_1280x720.mp4"));
    QCOMPARE(model.index(0, 0).data(VideosModel::PathRole).toString(), QStringLiteral("/videos/qnx_jeep_video_june_2012_1280x720.mp4"));

    // test resetting the model
    model.update();
    QCOMPARE(model.rowCount(), 1);

    // test additional functions
    int id = model.idByPath(QStringLiteral("/videos/qnx_jeep_video_june_2012_1280x720.mp4"));
    QCOMPARE(id, 0);
    id = model.idByPath(QStringLiteral("/foo/bar"));
    QCOMPARE(id, -1);
}

void tst_MediaPlayerModels::testPlaylistModel()
{
    // the PlaylistModel requires some PPS objects to be alive
    // hence, initialize the simulator here
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    QSqlDatabase db = DatabaseManager::database(QStringLiteral("mme.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    PlaylistModel model(AUDIOPLAYER_PLAYERNAME);
    model.setDatabase(db);

    // test unfiltered results
    QCOMPARE(model.rowCount(), 17);

    // test QML role names
    const QHash<int, QByteArray> roleNames = model.roleNames();
    QCOMPARE(roleNames[PlaylistModel::FileIdRole], QByteArrayLiteral("fid"));
    QCOMPARE(roleNames[PlaylistModel::TitleRole], QByteArrayLiteral("title"));
    QCOMPARE(roleNames[PlaylistModel::DurationRole], QByteArrayLiteral("duration"));
    QCOMPARE(roleNames[PlaylistModel::AlbumRole], QByteArrayLiteral("album"));
    QCOMPARE(roleNames[PlaylistModel::ArtistRole], QByteArrayLiteral("artist"));
    QCOMPARE(roleNames[PlaylistModel::ArtworkRole], QByteArrayLiteral("artwork"));

    // test data
    const QModelIndex index = model.index(0, 0);
    QCOMPARE(index.data(PlaylistModel::FileIdRole).toInt(), 1);
    QCOMPARE(index.data(PlaylistModel::TitleRole).toString(), QStringLiteral("Bo Bo's Groove"));
    QCOMPARE(index.data(PlaylistModel::DurationRole).toInt(), 318411);
    QCOMPARE(index.data(PlaylistModel::AlbumRole).toString(), QStringLiteral("Raising The Roof!"));
    QCOMPARE(index.data(PlaylistModel::ArtistRole).toString(), QStringLiteral("tomprincipato"));
    // verify that we get the full icon here (no thumbnail!)
    QCOMPARE(index.data(PlaylistModel::ArtworkRole).toString(), QStringLiteral("file:///apps/mediasources/imagecache//mme/2/original"));

    // test resetting the model
    model.update();
    QCOMPARE(model.rowCount(), 17);
}

void tst_MediaPlayerModels::testSourcesModel()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    SourcesModel model;
    QCOMPARE(model.rowCount(), 10);

    // test data
    QModelIndex index = model.index(9, 0);
    QCOMPARE(index.data(SourcesModel::DbPathRole).toString(), QStringLiteral("/dev/qdb/myusb"));
    QCOMPARE(index.data(SourcesModel::DeviceTypeRole).toString(), QStringLiteral("usb"));
    QCOMPARE(index.data(SourcesModel::FileSystemRole).toString(), QStringLiteral("dos (fat32)"));
    QCOMPARE(index.data(SourcesModel::ImagePathRole).toString(), QStringLiteral("/apps/mediasources/imagecache/myusb"));
    QCOMPARE(index.data(SourcesModel::MountPointRole).toString(), QStringLiteral("/fs/usb0"));
    QCOMPARE(index.data(SourcesModel::NameRole).toString(), QStringLiteral("My USB"));
    QCOMPARE(index.data(SourcesModel::SyncStatusRole).toBool(), true);

    // test values modification
    {
        QSignalSpy simulatorSpy(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)));
        QVERIFY(QPps::Simulator::self()->registerClient(QStringLiteral("/pps/services/mm-detect/status"), &model));
        QVERIFY(QPps::Simulator::self()->clientSetAttribute(&model, QStringLiteral("myusb"),
                                                            QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myusbNEW\",\"mount\":\"/fs/usb0NEW\",\"name\":\"My USB NEW\",\"fs_type\":\"dos (fat32 NEW)\",\"device_type\":\"usb NEW\",\"image_path\":\"/apps/mediasources/imagecache/myusb NEW\",\"synched\":false}"), QByteArrayLiteral("json")));

        QCOMPARE(simulatorSpy.count(), 1);
        QCOMPARE(simulatorSpy.at(0).at(1).toString(), QStringLiteral("myusb"));
        QCOMPARE(simulatorSpy.at(0).at(2).toString(), QStringLiteral("{\"dbpath\":\"/dev/qdb/myusbNEW\",\"mount\":\"/fs/usb0NEW\",\"name\":\"My USB NEW\",\"fs_type\":\"dos (fat32 NEW)\",\"device_type\":\"usb NEW\",\"image_path\":\"/apps/mediasources/imagecache/myusb NEW\",\"synched\":false}"));

        // test data
        index = model.index(9, 0);
        QCOMPARE(index.data(SourcesModel::DbPathRole).toString(), QStringLiteral("/dev/qdb/myusbNEW"));
        QCOMPARE(index.data(SourcesModel::DeviceTypeRole).toString(), QStringLiteral("usb NEW"));
        QCOMPARE(index.data(SourcesModel::FileSystemRole).toString(), QStringLiteral("dos (fat32 NEW)"));
        QCOMPARE(index.data(SourcesModel::ImagePathRole).toString(), QStringLiteral("/apps/mediasources/imagecache/myusb NEW"));
        QCOMPARE(index.data(SourcesModel::MountPointRole).toString(), QStringLiteral("/fs/usb0NEW"));
        QCOMPARE(index.data(SourcesModel::NameRole).toString(), QStringLiteral("My USB NEW"));
        QCOMPARE(index.data(SourcesModel::SyncStatusRole).toBool(), false);

        QPps::Simulator::self()->unregisterClient(&model);
    }

    //remove attribute
    {
        QVERIFY(QPps::Simulator::self()->registerClient(QStringLiteral("/pps/services/mm-detect/status"), &model));
        QVERIFY(QPps::Simulator::self()->clientRemoveAttribute(&model, QStringLiteral("myusb")));
        QCOMPARE(model.rowCount(), 9);

        index = model.index(0, 0);
        QCOMPARE(index.data(SourcesModel::NameRole).toString(), QStringLiteral("Radio"));
        index = model.index(1, 0);
        QCOMPARE(index.data(SourcesModel::NameRole).toString(), QStringLiteral("Juke Box"));
        index = model.index(2, 0);
        QCOMPARE(index.data(SourcesModel::NameRole).toString(), QStringLiteral("My other USB"));

        QPps::Simulator::self()->unregisterClient(&model);
    }
}

QTEST_MAIN(tst_MediaPlayerModels)
#include "tst_mediaplayermodels.moc"
