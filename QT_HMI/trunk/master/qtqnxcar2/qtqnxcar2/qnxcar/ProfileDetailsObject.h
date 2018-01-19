#ifndef QTQNXCAR2_PROFILEDETAILSOBJECT_H
#define QTQNXCAR2_PROFILEDETAILSOBJECT_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {
    struct ProfileData;

/**
 * Wrapper for exposing profile data to QML
 *
 * @sa ProfileManager
 */
class QTQNXCAR2_EXPORT ProfileDetailsObject : public QObject
{
    Q_OBJECT

    /**
     * The user's full name
     *
     * @accessors fullName(), setFullName()
     * @sa User::fullName()
     */
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY changed)

    /**
     * The Bluetooth identifier/address of the device preferred by the profile
     *
     * @accessors deviceId(), setDeviceId()
     * @sa ProfileMode::DeviceIdRole, BluetoothDevicesModel::AddressRole
     */
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY changed)

    /**
     * Theme identifier
     *
     * @accessors theme(), setTheme()
     * @sa ThemeListModel::IdRole
     */
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY changed)

    /**
     * Avatar identifier
     *
     * @accessors avatar(), setAvatar()
     * @sa AvatarListModel::IdRole
     */
    Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY changed)

public:
    /**
     * Creates an empty wrapper
     * @param parent the QObject parent
     */
    explicit ProfileDetailsObject(QObject *parent = 0);

    /// Destroys the instance
    ~ProfileDetailsObject();

    /**
     * Sets the profile data to expose to QML
     *
     * @param profile the data to expose
     * @sa profile
     */
    void setProfile(const ProfileData &profile);

    /**
     * Returns the currently exposed profile data
     *
     * @return the exposed data
     * @sa setProfile()
     */
    ProfileData profile() const;

    /// @sa #fullName
    QString fullName() const;
    /// @sa #fullName
    void setFullName(const QString &fullName);
    /// @sa #deviceId
    QString deviceId() const;
    /// @sa #deviceId
    void setDeviceId(const QString &deviceId);
    /// @sa #theme
    QString theme() const;
    /// @sa #theme
    void setTheme(const QString &theme);
    /// @sa #avatar
    QString avatar() const;
    /// @sa #avatar
    void setAvatar(const QString &avatar);

Q_SIGNALS:
    void changed();

private:
    class Private;
    Private *const d;
};

}

#endif
