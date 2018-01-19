#ifndef QTQNXCAR2_DATABASEMANAGER_H
#define QTQNXCAR2_DATABASEMANAGER_H

#include <QObject>

#include "qtqnxcar2_export.h"

class QSqlDatabase;

namespace QnxCar {

/**
 * Contains helper functions for retrieving databases
 */
class QTQNXCAR2_EXPORT DatabaseManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Creates a database manager
     * @param parent QObject instance to use as parent
     */
    explicit DatabaseManager(QObject *parent = 0);

    /**
     * Return platform-specific global database directory
     *
     * (e.g. @c /db on the target, @c SOURCE/simulator/data/databases on the host)
     */
    static QString databaseDirectory();

    /**
     * Return the database path for a specific database
     */
    static QString databasePath(const QString &database);

    /**
     * Retrieve the connection to a database identified by @p fileName
     */
    static QSqlDatabase database(const QString &fileName);

    /**
     * Retrieve the connection to a database identified by its QDB path
     */
    static QSqlDatabase forQdbPath(const QString &path);
};

}

#endif
