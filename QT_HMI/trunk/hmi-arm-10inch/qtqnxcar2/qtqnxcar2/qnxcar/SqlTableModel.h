#ifndef QTQNXCAR2_SQLTABLEMODEL_H
#define QTQNXCAR2_SQLTABLEMODEL_H

#include <QSqlTableModel>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Base class for QML adapter models working on an SQL table
 *
 * SqlTableModel provides the basic mapping of SQL table columns to property names required
 * for use of the model with QtQuick ListView.
 *
 * Derived classes then only need to provide those names through implementing columnNames().
 *
 * E.g.
 * @code
 * QList<QByteArray> ProfileModel::columnNames() const
 * {
 *     return QList<QByteArray>() << "id" << "full_name" << "device_id"
 *                                << "theme" << "avatar" << "avatar_file_path";
 * }
 * @endcode
 *
 * Such subclasses, if they are part of the library but need data members of their own,
 * can derive their own "Private" data class from SqlTableModel::Private and set an instance
 * of it via the protected constructor.
 * This instance is owned by SqlTableModel and can be accessed by the subclass through d_func()
 *
 * @sa SqlQueryModel
 */
class QTQNXCAR2_EXPORT SqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    /**
     * Constructor for use in sub classes that do not need their own member data
     *
     * Creates a SqlQueryModel::Private instance
     * @param parent the QObject parent
     * @param db the database connection
     */
    explicit SqlTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    /**
     * Destroys the instance
     *
     * Deletes the Private
     */
    ~SqlTableModel();

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
     * @param item its row() value is used to identify the row of the SQL table
     * @param role mapped to the column in the SQL table based on columnNames()
     * @return the data addressed by @p item and @p role or an invalid QVariant
     */
    QVariant data(const QModelIndex &item, int role) const Q_DECL_OVERRIDE;

protected:
    class Private;

    /**
     * Constructor to use in sub classes that require their own private data
     *
     * SqlTableModel takes ownership of @p derivedPrivate and deletes it upon destruction.
     *
     * @param derivedPrivate instance holding the derived class' data
     * @param parent the QObject parent
     * @param db the database connection
     * @see d_func()
     */
    explicit SqlTableModel(Private *derivedPrivate, QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    /**
     * "Empty" implementation, logs error if there was one
     */
    virtual void queryChange() Q_DECL_OVERRIDE;

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
