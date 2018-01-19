#include "object.h"
#include "simulator_object_p.h"

#include "changeset.h"
#include "simulator.h"

#include <QMap>
#include <QStringList>

using namespace QPps;

Object::Private::Private(const QString &path, Object::PublicationMode mode, QObject *parent)
    : QObject(parent)
    , m_isValid(false)
    , m_attributeCacheEnabled(false)
    , m_ppsObjectPath(path)
    , m_mode(mode)
{
    m_isValid = Simulator::self()->registerClient(m_ppsObjectPath, this, &m_errorString);
}

// TODO: for now attributesChanged is only called with a single changeset. We should be able to carry more complex changesets.
void Object::Private::notifyAttributeChanged(const QString &key, const QByteArray &value, const QByteArray &encoding)
{
    emit qobject_cast<Object*>(parent())->attributeChanged(key, Variant(value, encoding));

    Changeset singleChangeSet;
    singleChangeSet.assignments.insert(key, Variant(value, encoding));
    emit qobject_cast<Object*>(parent())->attributesChanged(singleChangeSet);
}

void Object::Private::notifyAttributeRemoved(const QString &key)
{
    emit qobject_cast<Object*>(parent())->attributeRemoved(key);

    Changeset singleChangeSet;
    singleChangeSet.removals << key;
    emit qobject_cast<Object*>(parent())->attributesChanged(singleChangeSet);
}

Object::Object(const QString &path, PublicationMode mode, QObject *parent)
    : QObject(parent)
    , d(new Private(path, mode, this))
{
}

Object::~Object()
{
    Simulator::self()->unregisterClient(d);
}

bool Object::attributeCacheEnabled() const
{
    return d->m_attributeCacheEnabled;
}

void Object::setAttributeCacheEnabled(bool enabled)
{
    d->m_attributeCacheEnabled = enabled;
    if (d->m_attributeCacheEnabled)
        Simulator::self()->triggerInitialListing(d);
}

bool Object::isValid() const
{
    return d->m_isValid;
}

QString Object::errorString() const
{
    return d->m_errorString;
}

QString Object::path() const
{
    return d->m_ppsObjectPath;
}

Variant Object::attribute(const QString &name, const Variant &defaultAttribute) const
{
    if (!d->m_attributeCacheEnabled)
        return defaultAttribute;

    // in case we are not subscribed, return invalid attributes for any key
    if (!(d->m_mode == SubscribeMode || d->m_mode == PublishAndSubscribeMode)) {
        return Variant();
    }

    QByteArray value;
    QByteArray encoding;

    const bool ok = Simulator::self()->clientGetAttribute(d, name, value, encoding);
    if (!ok)
        return defaultAttribute;

    return Variant(value, encoding);
}

QStringList Object::attributeNames() const
{
    if (!d->m_attributeCacheEnabled)
        return QStringList();
    else
        return Simulator::self()->clientGetAttributeNames(d);
}

bool Object::setAttributes(const Changeset &changeset)
{
    QMap<QString, Variant>::ConstIterator it = changeset.assignments.constBegin();
    for (; it != changeset.assignments.constEnd(); ++it) {
        if (!setAttribute(it.key(), it.value()))
            return false;
    }

    Q_FOREACH(const QString &removedAttr, changeset.removals) {
        if (!removeAttribute(removedAttr))
            return false;
    }

    return true;
}

bool Object::setAttribute(const QString &name, const Variant &attribute)
{
    if (!(d->m_mode == PublishMode || d->m_mode == PublishAndSubscribeMode)) {
        d->m_errorString = tr("Not allowed to write back attribute: %1").arg(name);
        return false;
    }

    return Simulator::self()->clientSetAttribute(d, name, attribute.value(), attribute.encoding());
}

bool Object::removeAttribute(const QString &name)
{
    if (!(d->m_mode == PublishMode || d->m_mode == PublishAndSubscribeMode)) {
        d->m_errorString = tr("Not allowed to remove attribute: %1").arg(name);
        return false;
    }

    return Simulator::self()->clientRemoveAttribute(d, name);
}
