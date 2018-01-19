#ifndef QTQNXCAR2_PROFILEMANAGER_H
#define QTQNXCAR2_PROFILEMANAGER_H

#include "qtqnxcar2_export.h"

#include <QObject>


namespace QnxCar {
struct ProfileData;
class ProfileDetailsObject;
class ProfileModel;

/**
 * Interface for components to store and restore profile-specific settings
 */
class QTQNXCAR2_EXPORT ProfileSettingsListener {
public:
    virtual ~ProfileSettingsListener();

    virtual void saveSettings(QMap<QString,QString> &settings) const;
    virtual void restoreSettings(const QMap<QString,QString> &settings);
};

/**
 * @brief Manages user profiles
 *
 * The ProfileManager class provides read-write access to profile data and
 * synchronizes profile-related data between PPS and the QDB database.
 * \note ProfileManager writes to the database and PPS, so you should not try to change
 *       profile data (user / theme) in any other way while a ProfileManager exists.
 */

class QTQNXCAR2_EXPORT ProfileManager : public QObject
{
    Q_OBJECT

    /**
     * The identifier for the active profile
     *
     * @accessors activeProfileId(), setActiveProfileId()
     * @sa #activeProfile, ProfileModel::IdRole
     */
    Q_PROPERTY(int activeProfileId READ activeProfileId WRITE setActiveProfileId NOTIFY activeProfileIdChanged)

    /**
     * Wrapper object filled with the active profile's data
     *
     * @accessors activeProfile()
     * @sa #activeProfileId
     */
    Q_PROPERTY(QnxCar::ProfileDetailsObject* activeProfile READ activeProfile CONSTANT)

    /**
     * Model containing all profiles
     *
     * @accessors model()
     */
    Q_PROPERTY(QnxCar::ProfileModel* model READ model CONSTANT)

public:
    /**
     * Loads the initial profile from PPS and sets up PPS monitoring
     * @param model the profile model to work on
     * @param parent the QObject parent
     */
    explicit ProfileManager(ProfileModel *model, QObject *parent = 0);

    /// Destroys the instance, stops PPS monitoring
    ~ProfileManager();

    /// @sa #activeProfileId
    int activeProfileId() const;

    /**
     * Set the active profile
     *
     * @return @c rrue in case the profile was switched, otherwise @c false
     * @sa #activeProfileId
     */
    bool setActiveProfileId(int id);

    ProfileDetailsObject *activeProfile() const;

    /**
     * Registers a settings listener
     */
    void addSettingsListener(ProfileSettingsListener *listener);

    /**
     * Deregisters a settings listener
     */
    void removeSettingsListener(ProfileSettingsListener *listener);

    /**
     * Adds another profile to the underlying model
     */
    Q_INVOKABLE int addProfile();

    /**
     * Remove the active profile
     *
     * This functions takes care of switching the profile to another one before
     * actually removing the profile from the underlying model.
     *
     * @note If there's only one profile left, this function will do nothing and return @c false
     *
     * @return @c true of the profile was successfully removed from the model,
     *         @c false if the active profile is the last profile
     */
    Q_INVOKABLE bool removeActiveProfile();

    /// @sa #model
    ProfileModel *model() const;

Q_SIGNALS:
    void activeProfileIdChanged();

private Q_SLOTS:
    /// triggered when the profile details object was modified
    void slotProfileDetailsObjectModified();

private:
    /**
     * Sync profile model with data from PPS
     *
     * E.g. in case PPS holds a user that is not in the database,
     * this will create a new profile in the database
     */
    void persistActiveProfile();

    /// Retrieve a profile stored in the model for @p id
    ProfileData storedProfile(int id) const;

    void loadProfileFromPps();
    void updatePpsFromProfile(const ProfileData &profile);

private:
    class Private;
    Private *const d;
};

}

#endif
