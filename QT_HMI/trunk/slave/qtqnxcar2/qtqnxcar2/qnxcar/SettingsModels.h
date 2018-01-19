#ifndef QTQNXCAR2_SETTINGSMODELS_H
#define QTQNXCAR2_SETTINGSMODELS_H

#include "qtqnxcar2_export.h"

#include <QAbstractItemModel>

namespace QPps {
class Object;
class Variant;
}

namespace QnxCar {
class BluetoothDevicesModel;
class User;

/**
 * Model for exposing theme data to QML
 *
 * @sa Theme
 */
class QTQNXCAR2_EXPORT ThemeListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /// Item roles
    enum Roles {
        /// Theme identifier , role name @c "id"
        IdRole = Qt::UserRole+1,
        /// Theme title, role name @c "title"
        TitleRole,
        /// Theme package name, role name @c "themePackageName"
        ThemePackageNameRole,
    };

    /**
     * Creates and populates model from PPS @c "/pps/qnxcar/themes"
     *
     * @param parent the QObject parent
     */
    explicit ThemeListModel(QObject* parent = 0);
    ~ThemeListModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const;

    /**
     * Returns the theme identifier at a given @p row
     *
     * @param row the index in the list
     * @return the identifier of the theme in @p row or QString()
     * @sa rowForThemeId(), #IdRole
     */
    Q_INVOKABLE QString idAt(int row) const;

    /**
     * Returns the row for a given theme @p id
     *
     * @param id the identifier of a theme
     * @return the row of the theme with the given @p id or @c -1
     * @sa idAt()
     */
    Q_INVOKABLE int rowForThemeId(const QString& id) const;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);

private:
    void reloadData();

    struct ThemeData;
    ThemeData dataAt(int row) const;
    ThemeData dataFromAttributeValue(const QPps::Variant& attribute) const;

private:
    class Private;
    Private *const d;
};

/**
 * Model for exposing Avatar image identifiers to QML
 *
 * The HTML5 version uses hardcoded values for the avatar images as well
 *
 * @see html5/webworks/apps/CarControl/personalization.html
 */
class QTQNXCAR2_EXPORT AvatarListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /// Item roles
    enum Roles {
        /// Image identifier, role name @c "id"
        IdRole = Qt::UserRole+1,
        /// Image label, role name @c "name"
        NameRole
    };

    /**
     * Creates and populates the model
     * @param parent the QObject parent
     */
    explicit AvatarListModel(QObject* parent = 0);
    ~AvatarListModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const;

    /**
     * Returns the avatar identifier at a given @p row
     *
     * @param row the index in the list
     * @return the identifier of the avatar in @p row or QString()
     * @sa rowForAvatarId(), #IdRole
     */
    Q_INVOKABLE QString idAt(int row) const;

    /**
     * Returns the row for a given avatar @p id
     *
     * @param id the identifier of a avatar
     * @return the row of the avatar with the given @p id or @c -1
     * @sa idAt()
     */
    Q_INVOKABLE int rowForAvatarId(const QString& id) const;

private:
    void loadData();

    struct AvatarImageData;
    AvatarImageData dataAt(int row) const;

private:
    class Private;
    Private *const d;
};

/**
 * Wrapper object for exposing multiple settings related models to QML
 *
 * @sa AvatarListModel, ThemeListModel, BluetoothDevicesModel
 */
class QTQNXCAR2_EXPORT SettingsModels : public QObject
{
    Q_OBJECT

    /**
     * Model for avatar data
     *
     * @accessors avatarListModel()
     */
    Q_PROPERTY(QnxCar::AvatarListModel* avatarListModel READ avatarListModel CONSTANT)

    /**
     * Model for theme data
     *
     * @accessors themeListModel()
     */
    Q_PROPERTY(QnxCar::ThemeListModel* themeListModel READ themeListModel CONSTANT)

    /**
     * Model containing known Bluetooth devices
     *
     * @accessors bluetoothDevicesModel()
     */
    Q_PROPERTY(QnxCar::BluetoothDevicesModel* bluetoothDevicesModel READ bluetoothDevicesModel CONSTANT)

public:
    explicit SettingsModels(QObject* parent = 0);
    ~SettingsModels();

    /// #avatarListModel
    AvatarListModel* avatarListModel() const;
    /// #themeListModel
    ThemeListModel* themeListModel() const;
    /// #bluetoothDevicesModel
    BluetoothDevicesModel *bluetoothDevicesModel() const;

private:
    class Private;
    Private *const d;
};

}

#endif
