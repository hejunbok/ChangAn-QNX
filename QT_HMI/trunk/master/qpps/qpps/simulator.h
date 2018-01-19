#ifndef QPPS_SIMULATOR_H
#define QPPS_SIMULATOR_H

#include "qpps_export.h"

#include <QObject>
#include <QHash>

namespace QPps {

class SimulatorPrivate;

/**
 * @short A class that simulates a PPS service
 *
 * When QPPS is build in simulator mode, all accesses to the PPS
 * file system a redirected to this simulator object.
 * The simulator behaves similar to a real PPS service (with some limitations)
 * but allows to introspect the written values or to modify them.
 */
class QPPS_EXPORT Simulator : public QObject
{
    Q_OBJECT

public:
    /**
     * Returns the global instance of the simulator.
     */
    static Simulator* self();

    // API for simulator objects

    /**
     * Registers the @p client for the @p objectPath.
     *
     * The client object will be notified for changes to this PPS object.
     *
     * Returns @c true on success or @c false if an error occurred.
     * In the latter case the error message is placed in @p errorMessage.
     */
    bool registerClient(const QString &objectPath, QObject *client, QString *errorMessage = 0);

    /**
     * Unregisters the @p client from the object path.
     *
     * \sa registerClient
     */
    void unregisterClient(QObject *client);

    /**
     * Triggers the initial listing of all attributes.
     */
    void triggerInitialListing(QObject *client);

    /**
     * Returns the names of all attributes in the PPS object the @p client has registered for.
     */
    QStringList clientGetAttributeNames(QObject *client) const;

    /**
     * Sets the attribute in the PPS object the @p client has registered for.
     *
     * Returns @c true on success, @c false otherwise.
     */
    bool clientSetAttribute(QObject *client, const QString &name, const QByteArray &value, const QByteArray &encoding);

    /**
     * Receives the @p value and @p encoding of the attribute in the PPS object the @p client has registered for.
     *
     * Returns @c true on success, @c false otherwise.
     */
    bool clientGetAttribute(QObject *client, const QString &name, QByteArray &value, QByteArray &encoding) const;

    /**
     * Removes the attribute in the PPS object the @p client has registered for.
     *
     * Returns @c true on success, @c false otherwise.
     */
    bool clientRemoveAttribute(QObject *client, const QString &name);


    /// API for unit tests

    /**
     * Inserts a @p value of the given @p encoding to the object specified by @p objectPath.
     */
    void insertAttribute(const QString &objectPath, const QString &key, const QByteArray &value, const QByteArray &encoding);

    /**
     * Inserts a @p objectPath
     */
    void insertObject(const QString &objectPath);

    /**
     * Clears all PPS objects inside the simulator.
     */
    void reset();

    /**
     * Returns the list of currently known PPS objects.
     */
    QMap<QString, QVariantMap> ppsObjects() const;

    /**
     * Dump the contents of the simular to the debug output
     *
     * Will only print entries that start with @p pathPrefix
     */
    void dumpTree(const QString& pathPrefix = QString());

Q_SIGNALS:
    /**
     * This signal is emitted whenever a new client has registered at the simulator.
     */
    void clientConnected(qulonglong client);

    /**
     * This signal is emitted whenever a client has unregistered from the simulator.
     */
    void clientDisconnected(qulonglong client);

    /**
     * This signal is emitted whenever an attribute has been added or changed.
     */
    void attributeChanged(const QString &objectPath, const QString &key, const QByteArray &value, const QByteArray &encoding);

    /**
     * This signal is emitted whenever an attribute has been removed
     */
    void attributeRemoved(const QString &objectPath, const QString &key);

    /**
     * This signal is emitted when a new PPS object was added.
     * There is currently no objectRemoved() signal because the simulator doesn't support removing objects.
     */
    void objectAdded(const QString &objectPath);

private:
    friend class SimulatorPrivate;

    Simulator();

    /**
     * Store \p attributes in d->m_ppsObjects and emit objectAdded() if the object was previously unknown.
     */
    void setObjectAttributes(const QString &objectPath, const QVariantMap &attributes);
    static Simulator* m_self;

    SimulatorPrivate *d;
};

}

#endif
