#ifndef QTQNXCAR2_APPLICATIONLISTMODEL_H
#define QTQNXCAR2_APPLICATIONLISTMODEL_H

#include "qtqnxcar2_export.h"

#include <QAbstractListModel>
#include <QSharedDataPointer>
#include <QString>

namespace QnxCar {

/**
 * Data associated with an application
 */
class QTQNXCAR2_EXPORT ApplicationData
{
public:
    /**
     * Creates an "empty" and invalid instance
     *
     * @sa isValid()
     */
    ApplicationData();
    ApplicationData(const ApplicationData &other);
    ~ApplicationData();

    ApplicationData &operator=(const ApplicationData &other);

    /**
     * Returns the application identifier
     *
     * @return an application identifier
     * @sa setId()
     */
    QString id() const;

    /**
     * Sets the application identifier
     *
     * @param id the application identifier
     * @sa id()
     */
    void setId(const QString &id);

    /**
     * Returns the application name
     *
     * @return an application name
     * @sa setName()
     */
    QString name() const;

    /**
     * Sets the application name
     *
     * @param id the application name
     * @sa name()
     */
    void setName(const QString &name);

    /**
     * Returns the application group
     *
     * @return an application group
     * @sa setGroup()
     */
    QString group() const;

    /**
     * Sets the application group
     *
     * @param id the application group
     * @sa group()
     */
    void setGroup(const QString &group);

    /**
     * Returns the application icon path
     *
     * @return an application icon path
     * @sa setIconPath()
     */
    QString iconPath() const;

    /**
     * Sets the application icon path
     *
     * @param id the application icon path
     * @sa iconPath()
     */
    void setIconPath(const QString &iconPath);

    /**
     * Returns the application URI
     *
     * @return an application URI
     * @sa setUri()
     */
    QString uri() const;

    /**
     * Sets the application URI
     *
     * @param id the application URI
     * @sa uri()
     */
    void setUri(const QString &uri);

    /**
     * Whether this is a valid application data object
     *
     * @return @c true if there is at least an identifier, see id()
     */
    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

QTQNXCAR2_EXPORT QDebug operator<<(QDebug dbg, const ApplicationData &data);

/**
 * Model listing available applications.
 *
 * This model lists the installed applications available to the user.
 * The application list is a flat list containing all applications
 * from all groups. To filter by application group, use ApplicationFilterModel.
 *
 * It is created and populated by Launcher.
 *
 * @sa Launcher::applicationListModel
 *
 */
class QTQNXCAR2_EXPORT ApplicationListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * Creates an empty model
     *
     * @param parent the QObject parent
     */
    explicit ApplicationListModel(QObject *parent = 0);
    ~ApplicationListModel();

    /// Item roles
    enum Roles {
        /// Application identifier, role name @c "appId"
        AppIdRole = Qt::UserRole + 1,
        /// Application name, role name @c "name"
        NameRole,
        /// Application group, role name @c "group"
        GroupRole,
        /// Application URI, role name @c "uri"
        UriRole,
        /// Application icon path, role name @c "iconPath"
        IconPathRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /**
     * Searches the model for a data object with a given name
     *
     * @param appName the application name to look for
     * @return the application's data object or an invalid instance
     * @sa #NameRole, ApplicationData::isValid()
     */
    ApplicationData findByName(const QString &appName) const;

    /**
     * Searches the model for a data object with a given identifier
     *
     * @param appId the application idenifier to look for
     * @return the application's data object or an invalid instance
     * @sa #AppIdRole, ApplicationData::isValid()
     */
    ApplicationData findById(const QString &appId) const;

    /**
     * The model contents as a vector
     *
     * @return a vector containing all data objects of the model
     * @sa setApplications(), addApplication(), removeApplication()
     */
    QVector<ApplicationData> applications() const;

    /**
     * Sets the model contents
     *
     * @param applications a vector of application data objects to expose through the model
     * @sa applications(), addApplication(), removeApplication()
     */
    void setApplications(const QVector<ApplicationData> &applications);

    /**
     * Adds or updates an application data object to the model
     *
     * @param application the data object to add
     * @sa applications(), setApplications(), removeApplication()
     */
    void addApplication(const ApplicationData &application);

    /**
     * Removes an application data object from the model
     *
     * @param appId the id of the data object to remove
     * @sa applications(), setApplications(), addApplication()
     */
    void removeApplication(const QString &appId);

private:
    class Private;
    Private *const d;
};

}

#endif
