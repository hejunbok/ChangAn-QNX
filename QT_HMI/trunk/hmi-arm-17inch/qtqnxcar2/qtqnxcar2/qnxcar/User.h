#ifndef QTQNXCAR2_USER_H
#define QTQNXCAR2_USER_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Wrapper class for the @c /pps/qnxcar/profile/user PPS object
 */
class QTQNXCAR2_EXPORT User : public QObject
{
    Q_OBJECT

    /**
     * Identifier for the user profile
     *
     * @accessors id(), setId()
     * @sa ProfileManager::activeProfileId
     */
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)

    /**
     * The user's full name
     *
     * @accessors fullName(), setFullName()
     */
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)

    /**
     * The identifier of the user's avatar
     *
     * @accessors avatar(), setAvatar()
     * @sa AvatarListModel::IdRole
     */
    Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)

public:
    explicit User(QObject *parent = 0);

    /// #id
    int id() const;
    /// #id
    void setId(int id);
    /// #fullName
    QString fullName() const;
    /// #fullName
    void setFullName(const QString &fullName);
    /// #avatar
    QString avatar() const;
    /// #avatar
    void setAvatar(const QString &avatar);

Q_SIGNALS:
    void idChanged(int id);
    void fullNameChanged(const QString &fullName);
    void avatarChanged(const QString &avatar);

private:
    class Private;
    Private* const d;
};

}

#endif
