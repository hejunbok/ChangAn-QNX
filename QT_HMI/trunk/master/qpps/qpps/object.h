#ifndef QPPS_OBJECT_H
#define QPPS_OBJECT_H

#include <QObject>

#include "variant.h"

namespace QPps
{

struct Changeset;

/**
 * \class Object
 *
 * \brief A PPS object in the /pps tree.
 *
 * A PPS <em>object</em> holds key-value pairs called <em>attributes</em>.
 * The PPS system allows changing attributes and subscribing to attribute changes in a
 * structured way. The state of a PPS object is saved across reboots by default.
 *
 * PPS objects look much like text files, and they appear in the file system, usually under /pps.
 * Object knows how to deal with those files so you can deal with PPS at the level of objects and
 * attributes.
 */
class QPPS_EXPORT Object : public QObject
{
    Q_OBJECT
public:
    /// Determines which way attribute changes will flow between the application and the PPS object
    enum PublicationMode
    {
        PublishMode = 0, ///< Publish but do not subscribe to attribute changes of the underlying PPS object
        SubscribeMode, ///< Subscribe to but do not publish attribute changes of the underlying PPS object
        ///< Publish and subscribe to attribute changes of the underlying PPS object
        /// Note: If you are opening a server object in client mode you still need to use "PublishAndSubscribeMode
        /// otherwise you will not connect properly to the object
        PublishAndSubscribeMode
    };

    /**
     * Create an Object representing the PPS object at @p path.
     *
     * If @p create is true PPS objecct will be created if not exists, if false PPS will not be
     * be created and error will be set if object doesn't exist.
     *
     * \sa isValid(), \enum PublicationMode
     */
    explicit Object(const QString &path, PublicationMode mode = PublishAndSubscribeMode, bool create = false, QObject *parent = 0);

    /**
     * Destroys the Object.
     */
    ~Object();


    /**
     * When this is \a true, just before an attributeChanged() signal is emitted, the
     * attribute's name and value are cached internally for later querying via attributeNames()
     * and attribute().
     * When it is \a false, no such caching takes place.
     * The default is false.
     *
     * \sa setAttributeCacheEnabled()
     */
    bool attributeCacheEnabled() const;

    /**
     * When \p cacheEnabled is \a true, attribute changes are cached internally for later querying via attributeNames()
     * and attribute().
     * When \p cacheEnabled is \a true, no such caching takes place <em>and all cached attributes are discarded</em>.
     *
     * \sa attributeCacheEnabled()
     */
    void setAttributeCacheEnabled(bool cacheEnabled);

    /**
     * Returns true when the underlying PPS object at path() has been opened successfully and there
     * was no error in the meantime, false otherwise.
     */
    bool isValid() const;

    /// Returns a string describing the last error, if any.
    QString errorString() const;

    /// Returns the path of the underlying PPS object.
    QString path() const;

    /**
     * Returns the value of attribute \p name, or \p defaultValue if no such attribute is known.
     *
     * \sa attributeCacheEnabled()
     */
    Variant attribute(const QString &name, const Variant &defaultValue = Variant()) const;

    /**
     * Returns the names of all known attributes in this PPS object.
     *
     * \sa attributeCacheEnabled()
     */
    QStringList attributeNames() const;

    /**
     * Writes a set of attribute assignments and removals atomically, so that subscribers will
     * not see a state with only some of the changes applied.
     *
     * \sa attributesChanged()
     */
    bool setAttributes(const Changeset &changes);

Q_SIGNALS:
    /**
     * Emitted when attributes in the underlying PPS object were changed or removed.
     * This keeps changes together that were written atomically.
     *
     * \sa setAttributes()
     */
    void attributesChanged(const QPps::Changeset &changes);

    /**
     * Emitted when an attribute in the underlying PPS object has changed.
     *
     * \p name The name of the attribute that has changed
     * \p attribute The new value of the attribute
     *
     * \note The PPS system makes no attempt to filter out no-op changes; it may be that attribute
     *       \p name was set, but only to the value it already had.
     */
    void attributeChanged(const QString &name, const QPps::Variant &value);

    /**
     * Emitted when the attribute \p name was removed from the underlying PPS object.
     */
    void attributeRemoved(const QString &name);

    /**
     * Emitted when the underlying PPS object was removed.
     * isValid() will return false after this has been emitted.
     */
    void objectRemoved();

public Q_SLOTS:
    /**
     * Sets the attribute \p name of the underlying PPS object to \p value.
     *
     * \note The PPS system makes no attempt to filter out no-op changes; if an attribute is set,
     *       all subscribers will be notified of that, even if the value didn't change.
     */
    bool setAttribute(const QString &name, const QPps::Variant &value);

    /**
     * Removes the attribute \p name from the underlying PPS object.
     */
    bool removeAttribute(const QString &name);

private:
    class Private;
    Private *d;
};

}

#endif
