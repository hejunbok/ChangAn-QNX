#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "qnxcar/SettingsModels.h"
#include "qnxcar/ProfileData_p.h"
#include "qnxcar/ProfileDetailsObject.h"
#include "qnxcar/ProfileManager.h"
#include "qnxcar/ProfileModel.h"
#include "qnxcar/User.h"
#include "qnxcar/Theme.h"
#include <qnxcar/DatabaseManager.h>

#include <qpps/simulator.h>

#include <QString>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace QnxCar;

static const QString DATABASE_CONNECTION_NAME = QStringLiteral("tst_SettingsModels");

static const QString SQL_CREATE_PROFILES_TABLE = QLatin1Literal(
    "CREATE TABLE \"profiles\" ("
        "\"id\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , "
        "\"full_name\" VARCHAR(20) NOT NULL , "
        "\"device_id\" VARCHAR(20) DEFAULT \"0\", "
        "\"theme\" VARCHAR(50) NOT NULL , "
        "\"avatar\" VARCHAR(50) NOT NULL , "
        "\"avatar_file_path\" VARCHAR(150))");

/**
 * Settings models test
 *
 * @note ProfileModel is a read-write model, hence we use
 * a in-memory database for testing.
 */
class tst_SettingsModels : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();

    void profileManager_UpdatePpsFromModel();
    void profileManager_SwitchProfile();
    void profileManager_TryRemoveLastProfile();
    void profileManager_SetInvalidProfile();

    void profilesListModel_Initial();
    void themesListModel_Initial();
    void avatarListModel_Initial();
};

void tst_SettingsModels::init()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), DATABASE_CONNECTION_NAME);
    db.setDatabaseName(QStringLiteral(":memory:"));
    QVERIFY(db.open());
    QSqlQuery query(db);
    bool success = query.exec(SQL_CREATE_PROFILES_TABLE);
    if (!success) {
        qWarning() << query.lastError();
    }
    QVERIFY(success);

    db.commit();
}

void tst_SettingsModels::cleanup()
{
    QSqlDatabase::removeDatabase(DATABASE_CONNECTION_NAME);
}

void tst_SettingsModels::profileManager_SwitchProfile()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    ProfileModel model(QSqlDatabase::database(DATABASE_CONNECTION_NAME));
    ProfileManager manager(&model);

    // the raw user object
    User user;
    int id = user.id();

    Theme theme;
    QString currentTheme = theme.property("current").toString();

    // by default, this profile has the following settings
    QCOMPARE(user.avatar(), QStringLiteral("male1"));
    QCOMPARE(currentTheme, QStringLiteral("default"));

    int activeProfile = manager.activeProfileId();
    QCOMPARE(id, activeProfile);
    int originalNumberOfProfiles = model.rowCount();

    // add a new profile
    int newId = model.addProfile(QStringLiteral("Alice"), QStringLiteral("female1"), QStringLiteral("/some/path"), QStringLiteral("jeep"));
    QVERIFY(newId != -1);
    QCOMPARE(model.rowCount(), originalNumberOfProfiles+1);

    // activate the new profile
    QVERIFY(manager.setActiveProfileId(newId));
    QCOMPARE(manager.activeProfileId(), newId);

    // now check if the complete profile (user attributes, theme) has switched
    QCOMPARE(user.id(), newId);
    QCOMPARE(user.avatar(), QStringLiteral("female1"));
    QCOMPARE(user.fullName(), QStringLiteral("Alice"));
    QCOMPARE(theme.property("current").toString(), QStringLiteral("jeep"));
}

void tst_SettingsModels::profileManager_UpdatePpsFromModel()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    QPps::Simulator * const s = QPps::Simulator::self();
    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("id"),
                       QByteArrayLiteral("-1"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("avatar"),
                       QByteArrayLiteral("female2"), QByteArrayLiteral("s"));

    // the raw user object
    User user;
    QCOMPARE(user.id(), -1);
    QCOMPARE(user.fullName(), QStringLiteral("John Doe"));
    QCOMPARE(user.avatar(), QStringLiteral("female2"));

    // this should create a user profile
    ProfileModel model(QSqlDatabase::database(DATABASE_CONNECTION_NAME));
    ProfileManager manager(&model);

    // a new user in an empty database should have ID 1
    QCOMPARE(user.id(), 1);
    QCOMPARE(user.fullName(), QStringLiteral("Unnamed"));
    QCOMPARE(user.avatar(), QStringLiteral("male1"));

    manager.activeProfile()->setFullName(QStringLiteral("Jane"));
    QCOMPARE(user.fullName(), QStringLiteral("Jane"));

    manager.activeProfile()->setAvatar(QStringLiteral("female1"));
    QCOMPARE(user.avatar(), QStringLiteral("female1"));

    Theme theme;
    QCOMPARE(theme.property("current").toString(), QStringLiteral("default"));
    manager.activeProfile()->setTheme(QStringLiteral("lolcats"));
    QCOMPARE(theme.property("current").toString(), QStringLiteral("lolcats"));

    // other changes shouldn't screw this up...
    QCOMPARE(user.fullName(), QStringLiteral("Jane"));
}

void tst_SettingsModels::profileManager_TryRemoveLastProfile()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    ProfileModel model(QSqlDatabase::database(DATABASE_CONNECTION_NAME));
    ProfileManager manager(&model);

    QVERIFY(manager.activeProfileId() != -1);

    // the manager should refuse the remove the last active profile
    QVERIFY(!manager.removeActiveProfile());
}

void tst_SettingsModels::profileManager_SetInvalidProfile()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();


    ProfileModel model(QSqlDatabase::database(DATABASE_CONNECTION_NAME));
    ProfileManager manager(&model);

    int activeProfile = manager.activeProfileId();
    QVERIFY(activeProfile != -1);

    // try to make a profile active which does not exist
    QVERIFY(!manager.setActiveProfileId(42));

    QCOMPARE(manager.activeProfileId(), activeProfile);
}

void tst_SettingsModels::profilesListModel_Initial()
{
    ProfileModel model(QSqlDatabase::database(DATABASE_CONNECTION_NAME));

    QCOMPARE(model.rowCount(), 0);

    int id = model.addProfile(QStringLiteral("Bob"), QStringLiteral("male1"), QStringLiteral("/some/path"), QStringLiteral("default"));
    QVERIFY(id != -1);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.idAt(0), 1);
    QCOMPARE(model.dataAt(0).fullName, QStringLiteral("Bob"));
    QCOMPARE(model.dataAt(0).avatar, QStringLiteral("male1"));
    QCOMPARE(model.dataAt(0).theme, QStringLiteral("default"));

    // add items
    id = model.addProfile(QStringLiteral("Alice"), QStringLiteral("female1"), QStringLiteral("/some/path"), QStringLiteral("jeep"));
    QVERIFY(id != -1);
    QCOMPARE(model.rowCount(), 2);

    QCOMPARE(model.idAt(1), 2);
    QCOMPARE(model.dataAt(1).fullName, QStringLiteral("Alice"));
    QCOMPARE(model.dataAt(1).avatar, QStringLiteral("female1"));
    QCOMPARE(model.dataAt(1).avatarFilePath, QStringLiteral("/some/path"));
    QCOMPARE(model.dataAt(1).theme, QStringLiteral("jeep"));

    // remove items
    QVERIFY(model.removeProfile(id));
    QCOMPARE(model.rowCount(), 1);

    // try to remove invalid items
    QVERIFY(!model.removeProfile(42));
    QCOMPARE(model.rowCount(), 1);

    // add user from template
    id = model.addProfile();
    QVERIFY(id != -1);
    QCOMPARE(model.rowCount(), 2);

    QCOMPARE(model.idAt(1), 3);
    QCOMPARE(model.dataAt(1).fullName, QStringLiteral("Unnamed"));
    QCOMPARE(model.dataAt(1).avatar, QStringLiteral("male1"));
    QCOMPARE(model.dataAt(1).avatarFilePath, QStringLiteral(""));
    QCOMPARE(model.dataAt(1).theme, QStringLiteral("default"));
}

void tst_SettingsModels::themesListModel_Initial()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    ThemeListModel model;

    QCOMPARE(model.idAt(0), QStringLiteral("default"));
    const QModelIndex indexRow0 = model.index(0);
    QCOMPARE(indexRow0.data(ThemeListModel::TitleRole).toString(), QStringLiteral("Default"));
    QCOMPARE(indexRow0.data(ThemeListModel::ThemePackageNameRole).toString(), QStringLiteral("default"));

    QCOMPARE(model.idAt(1), QStringLiteral("jeep"));
    const QModelIndex indexRow1 = model.index(1);
    QCOMPARE(indexRow1.data(ThemeListModel::TitleRole).toString(), QStringLiteral("Jeep Theme"));
    QCOMPARE(indexRow1.data(ThemeListModel::ThemePackageNameRole).toString(), QStringLiteral("jeep"));
}


void tst_SettingsModels::avatarListModel_Initial()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    AvatarListModel model;

    int id = 0;
    QCOMPARE(model.idAt(id++), QStringLiteral("female1"));
    QCOMPARE(model.idAt(id++), QStringLiteral("female2"));
    QCOMPARE(model.idAt(id++), QStringLiteral("male1"));
    QCOMPARE(model.idAt(id++), QStringLiteral("male2"));
}

QTEST_MAIN(tst_SettingsModels)
#include "tst_settingsmodels.moc"
