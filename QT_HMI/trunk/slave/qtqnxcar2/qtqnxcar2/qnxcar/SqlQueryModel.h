#ifndef QTQNXCAR2_SQLQUERYMODEL_H
#define QTQNXCAR2_SQLQUERYMODEL_H

#include <QSqlQueryModel>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Base class for QML adapter models working on an SQL query
 *
 * SqlQueryModel provides the basic mapping of SQL query fields to property names required
 * for use of the model with QtQuick ListView.
 *
 * Derived classes then only need to provide those names, basically the columns of the queried
 * data through implementing columnNames().
 *
 * E.g.
 * @code
 * QList<QByteArray> AlbumsModel::columnNames() const
 * {
 *     return QList<QByteArray>() << "album_id" << "album" << "artwork";
 * }
 * @endcode
 *
 * Such subclasses, if they are part of the library but need data members of their own,
 * can derive their own "Private" data class from SqlQueryModel::Private and set an instance
 * of it via the protected constructor.
 * This instance is owned by SqlQueryModel and can be accessed by the subclass through d_func()
 *
 * E.g.
 * @code
 * void AlbumsModel::setFilterArtist(int artistId)
 * {
 *     AlbumsModel::Private *dptr = d_func<AlbumsModel::Private>();
 *     dptr->artistId = artistId;
 *     dptr->filterType = ArtistFilterType;
 *     update();
 * }
 * @endcode
 *
 * @sa SqlTableModel
 */
class QTQNXCAR2_EXPORT SqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    /**
     * Constructor for use in sub classes that do not need their own member data
     *
     * Creates a SqlQueryModel::Private instance
     * @param parent the QObject parent
     */
    explicit SqlQueryModel(QObject *parent = 0);

    /**
     * Destroys the instance
     *
     * Deletes the Private
     */
    ~SqlQueryModel();

    /**
     * Returns the database connection used by the model
     * @return the current database connection, invalid if none has been set before
     * @sa setDatabase()
     */
    QSqlDatabase database() const;

    /**
     * Sets the database connection to use
     * @param database connection to a database to use for querying
     * @sa database()
     */
    void setDatabase(const QSqlDatabase &database);

    /**
     * Maps roles to column/property names
     *
     * Starts at Qt::UserRole + 1, mapping one role for each entry of columnNames() result.
     */
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /**
     * Returns the data for a given row as specified by @p item and column as specified by @p role
     *
     * For use in QtQuick ListView, all columns have to be mapped to roles.
     * That mapping is provided by roleNames() and created based in input by the derived class
     * through columnNames()
     *
     * @param item its row() value is used to identify the nth record of the performed SQL query
     * @param role mapped to a field/column in the SQL record based on columnNames()
     * @return the data addressed by @p item and @p role or an invalid QVariant
     */
    QVariant data(const QModelIndex &item, int role) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    /**
     * Updated the model contents by calling runQuery()
     */
    void update();

protected:
    class Private;

    /**
     * Constructor to use in sub classes that require their own private data
     *
     * SqlQueryModel takes ownership of @p derivedPrivate and deletes it upon destruction.
     *
     * @param derivedPrivate instance holding the derived class' data
     * @param parent the QObject parent
     * @see d_func()
     */
    explicit SqlQueryModel(Private *derivedPrivate, QObject *parent = 0);

    /**
     * "Empty" implementation, logs error if there was one
     */
    void queryChange() Q_DECL_OVERRIDE;

    /**
     * Attempts to close and re-open the database provied by setDatabase()
     */
    void reopenDatabase();

    /**
     * Implement to provide the column names used for role/sql-column mapping
     *
     * Provide one name per derived model's role, see roleNames()
     *
     * @note Those names are also used to address the data columns through QML.
     *
     * @return list of column names, one for each role
     */
    virtual QList<QByteArray> columnNames() const = 0;

    /**
     * Implement to perform the actual SQL query
     *
     * E.g. prepare a QSqlQuery object and call QSqlQueryModel::setQuery()
     */
    virtual void runQuery() = 0;

    /**
     * Non-const accessor to the Private object
     */
    template<class T>
    T *d_func() {
        return static_cast<T*>(d);
    }

    /**
     * Const accessor to the Private object
     */
    template<class T>
    const T *d_func() const {
        return static_cast<const T*>(d);
    }

private:
    Private *const d;
};

}

#endif
