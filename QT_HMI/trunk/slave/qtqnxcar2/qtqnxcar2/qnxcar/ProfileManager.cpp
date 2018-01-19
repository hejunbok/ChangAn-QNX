#include "ProfileManager.h"

#include "ProfileData_p.h"
#include "ProfileDetailsObject.h"
#include "ProfileModel.h"
#include "SettingsModels.h"
#include "util/Util.h"

#include "qnxcar/User.h"
#include "qnxcar/Theme.h"

#include <QSet>

#include <QSqlError>
#include <QSqlQuery>

namespace QnxCar {

ProfileSettingsListener::~ProfileSettingsListener()
{
}

void ProfileSettingsListener::saveSettings(QMap<QString,QString> &) const
{
}

void ProfileSettingsListener::restoreSettings(const QMap<QString,QString> &)
{
}

class ProfileManager::Private
{
public:
    Private(ProfileModel *_model, ProfileManager *q)
        : user(new User(q))
        , theme(new Theme(q))
        , model(_model)
        , activeProfileId(-1)
        , activeProfile(new ProfileDetailsObject(q))
        , ignoreProfileDetailsModified(false)
    {}

    User* user;
    Theme* theme;
    ProfileModel* model;

    int activeProfileId;
    ProfileDetailsObject *activeProfile;
    bool ignoreProfileDetailsModified;
    QSet<ProfileSettingsListener*> settingsListeners;

    void saveSettings();
    void restoreSettings();
    void deleteSettings(int profileId);
};

void ProfileManager::Private::deleteSettings(int profileId)
{
    const QString queryString = "DELETE FROM settings WHERE profile_id = :profileId";
    QSqlQuery query(model->database());
    query.prepare(queryString);
    query.bindValue(QStringLiteral(":profileId"), profileId);
    if (!query.exec()) {
        qCritical("Query failed: %s (%s)", qPrintable(query.lastError().text()), qPrintable(queryString));
    }
}

void ProfileManager::Private::saveSettings()
{
    QMap<QString,QString> settings;
    Q_FOREACH(ProfileSettingsListener *i, settingsListeners)
        i->saveSettings(settings);

    //logic as in user.js
    const QString queryChunk1 = QLatin1String("INSERT OR REPLACE INTO settings (profile_id, key, value) ");

    QMap<QString,QString>::ConstIterator it = settings.constBegin();
    if (it == settings.constEnd())
        return;

    const QString profileId = QString::number(activeProfileId);

    //special treatment for the first entry
    QString queryChunk2 = QStringLiteral("SELECT %1 AS profile_id, '%2' AS key, '%3' AS value ").arg(profileId, it.key(), it.value());
    it++;

    for ( ; it != settings.constEnd(); ++it)
        queryChunk2 += QStringLiteral("UNION ALL SELECT %1, '%2', '%3' ").arg(profileId, it.key(), it.value());

    const QString queryString = queryChunk1 + queryChunk2;

    QSqlQuery query(model->database());
    query.prepare(queryString);
    if (!query.exec()) {
        qCritical("Query failed: %s (%s)", qPrintable(query.lastError().text()), qPrintable(queryString));
    }
}

void ProfileManager::Private::restoreSettings()
{
    const QString queryString = QStringLiteral("SELECT * FROM settings WHERE profile_id = :profileId");

    QSqlQuery query(model->database());
    query.prepare(queryString);
    query.bindValue(QStringLiteral(":profileId"), activeProfileId);
    if (!query.exec()) {
        qCritical("Query failed: %s (%s)", qPrintable(query.lastError().text()), qPrintable(queryString));
    }

    QMap<QString,QString> settings;

    while (query.next()) {
        const QString key = query.value(QStringLiteral("key")).toString();
        const QString value = query.value(QStringLiteral("value")).toString();
        settings.insert(key, value);
    }

    Q_FOREACH(ProfileSettingsListener *i, settingsListeners)
        i->restoreSettings(settings);
}

ProfileManager::ProfileManager(ProfileModel *model, QObject *parent)
    : QObject(parent)
    , d(new Private(model, this))
{
    connect(d->activeProfile, SIGNAL(changed()), SLOT(slotProfileDetailsObjectModified()));

    // Only this code writes to the profile because it's part of the setting module.
    // The information in PPS is read-only for other code. Because the database has more information
    // than PPS, it is the authoritative data source. What it doesn't have is the current user ID, so:
    // load user ID from PPS, the rest from the database, then sync from database to PPS.

    int id = d->user->id();
    if (!d->model->profileIds().contains(id)) {
        if (!d->model->profileIds().isEmpty()) {
            qWarning() << "user ID" << id << "from PPS in unknown in database, picking first user in the database";
        } else {
            qWarning() << "No users in database, creating a user profile";
            d->model->addProfile();
        }
        id = d->model->profileIds().first();
    }
    setActiveProfileId(id);
}

ProfileManager::~ProfileManager()
{
    delete d;
}

void ProfileManager::addSettingsListener(ProfileSettingsListener *listener)
{
    d->settingsListeners.insert(listener);
}

void ProfileManager::removeSettingsListener(ProfileSettingsListener *listener)
{
    d->settingsListeners.remove(listener);
}

void ProfileManager::persistActiveProfile()
{
    ProfileData data = d->activeProfile->profile();
    int id = d->user->id();
    if (id != d->activeProfileId) {
        qWarning() << "Oops, d->user->id() =" << id << "but d->activeProfileId =" << d->activeProfileId;
    }

    // update the profile in the model
    d->model->updateProfile(d->activeProfileId, data);
    updatePpsFromProfile(data);
}

ProfileDetailsObject *ProfileManager::activeProfile() const
{
    return d->activeProfile;
}

int ProfileManager::activeProfileId() const
{
    return d->activeProfileId;
}

void ProfileManager::slotProfileDetailsObjectModified()
{
    if (d->ignoreProfileDetailsModified)
        return;
    ProfileData profile = d->activeProfile->profile();
    persistActiveProfile();
}

ProfileData ProfileManager::storedProfile(int id) const
{
    int row = d->model->rowForId(id);
    return d->model->dataAt(row);
}

bool ProfileManager::setActiveProfileId(int id)
{
    if (d->activeProfileId == id)
        return false;

    if (!d->model->profileIds().contains(id)) {
        car2_debug() << "Profile ID does not exist:" << id;
        return false;
    }

    d->saveSettings();

    d->activeProfileId = id;
    const ProfileData data = storedProfile(d->activeProfileId);
    d->ignoreProfileDetailsModified = true;
    d->activeProfile->setProfile(data);
    d->ignoreProfileDetailsModified = false;

    // rewrite the current user profile
    updatePpsFromProfile(data);
    d->restoreSettings();

    emit activeProfileIdChanged();
    return true;
}

int ProfileManager::addProfile()
{
    return d->model->addProfile();
}

bool ProfileManager::removeActiveProfile()
{
    const QList<int> profileIds = d->model->profileIds();
    if (profileIds.size() <= 1) {
        car2_debug() << "Refusing to remove last active profile";
        return false;
    }

    // save active id for later removal
    const int activeId = activeProfileId();

    const bool ret = d->model->removeProfile(activeId);

    // switch to another profile - usually next, previous if there is no next profile
    int idx = profileIds.indexOf(activeId) + 1;
    idx -= (idx >= profileIds.count()) ? 2 : 0;
    setActiveProfileId(profileIds[idx]);

    d->deleteSettings(activeId);

    return ret;
}

ProfileModel *ProfileManager::model() const
{
    return d->model;
}

void ProfileManager::updatePpsFromProfile(const ProfileData &profile)
{
    d->user->setId(d->activeProfileId);
    d->user->setAvatar(profile.avatar);
    d->user->setFullName(profile.fullName);
    d->theme->setCurrent(profile.theme);
}

}
