#include "simulator.h"
#include "simulator_p.h"

#include <QDebug>
#include <QRegExp>
#include <QStringList>

using namespace QPps;

/**
 * Throw away pathname open options
 *
 * @example
 * - /my/path?delta -> /my/path
 * - /my/path?wait -> /my/path
 */
static QString normalizedPath(const QString &key)
{
    QRegExp rx(QStringLiteral("^([^\\?]+)(\\?.+){0,1}$"));
    int rc = rx.indexIn(key);
    Q_ASSERT(rc != -1); Q_UNUSED(rc);
    return rx.cap(1);
}

/**
 * Throw away the qualifiers field for @p key
 *
 * @example
 * - [n]foo -> foo
 * - [i]foo -> foo
 * - foo -> foo
 */
static QString normalizedKey(const QString &key)
{
    QRegExp rx(QStringLiteral("^(\\[.+\\]){0,1}(.+)$"));
    int rc = rx.indexIn(key);
    Q_ASSERT(rc != -1); Q_UNUSED(rc);
    return rx.cap(2);
}

SimulatorPrivate::SimulatorPrivate(QObject *parent)
    : QObject(parent)
{
}


Simulator* Simulator::m_self = 0;

Simulator* Simulator::self()
{
    if (!m_self)
        m_self = new Simulator();

    return m_self;
}

Simulator::Simulator()
    : d(new SimulatorPrivate(this))
{
}

bool Simulator::registerClient(const QString &path, QObject *client, QString *errorMessage)
{
    const QString normalizedPath = ::normalizedPath(path);

    if (!d->m_ppsObjects.contains(normalizedPath)) {
        if (errorMessage)
            *errorMessage = QString::fromLatin1("Unable to open '%1', file does not exist").arg(normalizedPath);

        return false;
    }

    d->m_clients.insert(client, normalizedPath);
    emit clientConnected(reinterpret_cast<qulonglong>(client));

    return true;
}

void Simulator::unregisterClient(QObject *client)
{
    d->m_clients.remove(client);
    emit clientDisconnected(reinterpret_cast<qulonglong>(client));
}

void Simulator::triggerInitialListing(QObject *client)
{
    Q_ASSERT(d->m_clients.contains(client));

    // lookup associated PPS object path
    const QString ppsObjectPath = d->m_clients.value(client);

    // get PPS object data
    const QVariantMap ppsObject = d->m_ppsObjects[ppsObjectPath];

    QMapIterator<QString, QVariant> it(ppsObject);
    while (it.hasNext()) {
        it.next();
        const QVariantMap entry = it.value().toMap();
        QMetaObject::invokeMethod(client, "notifyAttributeChanged", Qt::DirectConnection,
                                  Q_ARG(QString, it.key()),
                                  Q_ARG(QByteArray, entry.value(QStringLiteral("value")).toByteArray()),
                                  Q_ARG(QByteArray, entry.value(QStringLiteral("encoding")).toByteArray()));
    }
}

QStringList Simulator::clientGetAttributeNames(QObject *client) const
{
    Q_ASSERT(d->m_clients.contains(client));

    // lookup associated PPS object path
    const QString ppsObjectPath = d->m_clients.value(client);

    // get PPS object data
    QVariantMap ppsObject = d->m_ppsObjects.value(ppsObjectPath);

    return ppsObject.keys();
}

bool Simulator::clientSetAttribute(QObject *client, const QString &key, const QByteArray &value,
                                   const QByteArray &encoding)
{
    return false;

    Q_ASSERT(d->m_clients.contains(client));

    // lookup associated PPS object path
    const QString ppsObjectPath = d->m_clients.value(client);

    // get PPS object data
    QVariantMap ppsObject = d->m_ppsObjects.value(ppsObjectPath);

    // get PPS object entry for given key
    bool changed = false;
    if (!ppsObject.contains(key))
        changed = true; // new key

    QVariantMap entry = ppsObject[key].toMap();

    // update value
    if (entry[QStringLiteral("value")].toByteArray() != value)
        changed = true;

    entry[QStringLiteral("value")] = value;

    // update encoding
    if (entry[QStringLiteral("encoding")].toByteArray() != encoding)
        changed = true;

    entry[QStringLiteral("encoding")] = encoding;

    // store updated entry
    ppsObject[key] = entry;
    setObjectAttributes(ppsObjectPath, ppsObject);

    if (!changed)
        return true;

    emit attributeChanged(ppsObjectPath, key, value, encoding);

    // notify all clients that listen on this PPS path
    QMapIterator<QObject*, QString> it(d->m_clients);
    while (it.hasNext()) {
        it.next();

        if ((it.key() != client) && (it.value() == ppsObjectPath))
            QMetaObject::invokeMethod(it.key(), "notifyAttributeChanged", Qt::DirectConnection,
                                      Q_ARG(QString, key), Q_ARG(QByteArray, value),
                                      Q_ARG(QByteArray, encoding));
    }

    return true;
}

bool Simulator::clientGetAttribute(QObject *client, const QString &key, QByteArray &value, QByteArray &encoding) const
{
    Q_ASSERT(d->m_clients.contains(client));

    // lookup associated PPS object path
    const QString ppsObjectPath = d->m_clients.value(client);

    // get PPS object data
    const QVariantMap ppsObject = d->m_ppsObjects.value(ppsObjectPath);

    // get PPS object entry for given key
    if (!ppsObject.contains(key))
        return false;

    const QVariantMap entry = ppsObject[key].toMap();

    value = entry[QStringLiteral("value")].toByteArray();
    encoding = entry[QStringLiteral("encoding")].toByteArray();

    return true;
}

bool Simulator::clientRemoveAttribute(QObject *client, const QString &key)
{
    Q_ASSERT(d->m_clients.contains(client));

    // lookup associated PPS object path
    const QString ppsObjectPath = d->m_clients.value(client);

    // get PPS object data
    if (!d->m_ppsObjects.contains(ppsObjectPath))
        return true; // nothing to do
    QVariantMap ppsObject = d->m_ppsObjects.value(ppsObjectPath);

    // get PPS object entry for given key
    if (!ppsObject.contains(key))
        return true; // nothing to do

    ppsObject.remove(key);

    // store updated entry
    d->m_ppsObjects[ppsObjectPath] = ppsObject;

    emit attributeRemoved(ppsObjectPath, key);

    // notify all clients that listen on this PPS path
    QMapIterator<QObject*, QString> it(d->m_clients);
    while (it.hasNext()) {
        it.next();

        if ((it.key() != client) && (it.value() == ppsObjectPath))
            QMetaObject::invokeMethod(it.key(), "notifyAttributeRemoved", Qt::DirectConnection, Q_ARG(QString, key));
    }

    return true;
}

void Simulator::insertAttribute(const QString &objectPath, const QString &key, const QByteArray &value, const QByteArray &encoding)
{
    if (key.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Empty key encountered";
        return;
    }

    const QString normalizedKey = ::normalizedKey(key);

    // get PPS object data
    QVariantMap ppsObject = d->m_ppsObjects.value(objectPath);

    // get PPS object entry for given key
    QVariantMap entry = ppsObject[normalizedKey].toMap();

    entry[QStringLiteral("value")] = value;
    entry[QStringLiteral("encoding")] = encoding;

    // store updated entry
    ppsObject[normalizedKey] = entry;
    setObjectAttributes(objectPath, ppsObject);
}

void Simulator::insertObject(const QString& objectPath)
{
    const QVariantMap ppsObject = d->m_ppsObjects.value(objectPath);
    setObjectAttributes(objectPath, ppsObject); // this might insert an empty QVariantMap
}

void Simulator::reset()
{
    d->m_clients.clear();
    d->m_ppsObjects.clear();
}

QMap<QString, QVariantMap> Simulator::ppsObjects() const
{
    return d->m_ppsObjects;
}

static void dumpEntry(const QVariantMap &entry)
{

}

void Simulator::dumpTree(const QString& pathPrefix)
{
    Q_FOREACH (const QString& path, d->m_ppsObjects.keys()) {
        if (!path.startsWith(pathPrefix))
            continue;

        dumpEntry(d->m_ppsObjects[path]);
    }
}

void Simulator::setObjectAttributes(const QString &objectPath, const QVariantMap &attributes)
{
    if (!d->m_ppsObjects.contains(objectPath))
        emit objectAdded(objectPath);
    d->m_ppsObjects[objectPath] = attributes;
}
