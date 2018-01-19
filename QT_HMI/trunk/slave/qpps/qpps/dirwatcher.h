#ifndef QPPS_DIRWATCHER_H
#define QPPS_DIRWATCHER_H

#include "qpps_export.h"

#include <QObject>

namespace QPps
{

/**
 * \class DirWatcher
 *
 * \brief Watches object additions and removals in a directory.
 *
 * This class uses a feature of the PPS system for watching object addition and removal.
 * It can list the current objects and signals addition and removal events.
 */
class QPPS_EXPORT DirWatcher : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a DirWatcher watching object additions and removals at @p path.
     *
     * \sa isValid()
     */
    explicit DirWatcher(const QString &path, QObject *parent = 0);

    /**
     * Destroys the DirWatcher.
     */
    ~DirWatcher();

    /**
     * Returns true when the underlying PPS DirWatcher at path() has been opened successfully and there
     * was no error in the meantime, false otherwise.
     */
    bool isValid() const;

    /// Returns a string describing the last error, if any.
    QString errorString() const;

    /// Returns the path that is being watched.
    QString path() const;

    /**
     * Returns the names of all known objects in the watched path.
     *
     * \sa objectCacheEnabled()
     */
    QStringList objectNames() const;

Q_SIGNALS:

    /**
     * Emitted when the object \p name was added to the watched directory.
     */
    void objectAdded(const QString &name);

    /**
     * Emitted when the object \p name was removed from the watched directory.
     */
    void objectRemoved(const QString &name);

private:
    class Private;
    Private *d;
};

}

#endif
