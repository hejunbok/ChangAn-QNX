#ifndef QTQNXCAR2_PROFILEMODEL_H
#define QTQNXCAR2_PROFILEMODEL_H

#include "qtqnxcar2_export.h"

#include "SqlQueryModel.h"

namespace QnxCar {
struct ProfileData;

/**
 * Model listing configured user profiles.
 *
 * The model lists the contents of the personalization database.
 * The data is persisted and hence available across sessions.
 *
 * A model instance is provided by ProfileManager.
 *
 * @sa html5/webworks/tools/BB10webworks-1.0.2.9/Framework/ext/user/user.js
 * (getAllProfiles function, etc.)
 */
class QTQNXCAR2_EXPORT ProfileModel : public SqlQueryModel
{
    Q_OBJECT

public:
    /**
     * Item roles
     *
     * @note Matches the columns in the profiles table
     */
    enum Role {
        /// Profile ID (int), role name @c "id"
        IdRole = Qt::UserRole+1,
        /// Full name, (QString), role name @c "full_name"
        FullNameRole,
        /// Bluetooth device ID (QString), role name @c "device_id"
        DeviceIdRole,
        /// Theme name (QString), role name @c "theme"
        ThemeRole,
        /// Avatar ID (QString), role name @c "avatar"
        AvatarRole,
        /// Avatar file path (QString), role name @c "avatar_file_path"
        AvatarFilePathRole
    };

    /**
     * Creates an instance and tries to fill it through the given database connection
     *
     * @param parent the QObject parent
     * @param db the database connection
     */
    explicit ProfileModel(const QSqlDatabase &db, QObject *parent = 0);
    virtual ~ProfileModel();

    /**
     * Add a new profile to this model
     *
     * @return New id of the added profile
     */
    int addProfile(const ProfileData& data);

    Q_INVOKABLE int addProfile(const QString &fullName, const QString &avatar,
                               const QString &avatarFilePath, const QString &theme);
    /**
     * Creates a user from template
     *
     * Useful when adding new profiles from QML
     */
    Q_INVOKABLE int addProfile();
    Q_INVOKABLE bool removeProfile(int id);

    /// Fetch the profile data for @p id
    ProfileData profile(int id) const;

    /**
     * Retrieve the IDs registered in this model
     */
    QList<int> profileIds() const;

    /**
     * Update the data of profile with ID @p id
     *
     * @return @c true on success, else @c false
     */
    bool updateProfile(int id, const ProfileData &data);

    /// Returns -1 in case an invalid row was specified
    Q_INVOKABLE int idAt(int row) const;
    ProfileData dataAt(int row) const;
    Q_INVOKABLE int rowForId(int id) const;

protected:
    void runQuery() Q_DECL_OVERRIDE;
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;
};

}

#endif
