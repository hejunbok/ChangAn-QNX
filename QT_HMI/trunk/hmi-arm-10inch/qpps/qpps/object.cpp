#include "object.h"
#include "object_p.h"

#include <changeset.h>

#include <QList>
#include <QStringList>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace QPps;

static const int QPPS_DEFAULT_PPS_OBJECT_SIZE = 1 << 16;

static int openFlags(Object::PublicationMode mode, bool create)
{
    int flags = O_RDONLY;

    switch (mode) {
    case Object::PublishMode:
        flags = O_WRONLY; break;
    case Object::SubscribeMode:
        flags = O_RDONLY; break;
    case Object::PublishAndSubscribeMode:
        flags = O_RDWR; break;
    default:
        qWarning("Wrong publication mode specified %d , will be defaulted to O_RDONLY", mode);
        Q_ASSERT(false);
        break;
    }

    flags |= create?O_CREAT:0;

    return flags;
}

Object::Private::Private(Object *parent)
    : QObject(parent)
    , notifier(0)
    , cache(0)
{
}

Object::Private::~Private()
{
    delete cache;
    cache = 0;
}

bool Object::Private::writeChanges(const QByteArray &changes)
{
    const ssize_t written = ::write(notifier->socket(), changes.constData(), changes.length());
    const bool ok = (written == changes.length());

    if (!ok)
        errorString = QObject::tr("Unable to write data to %1: %2").arg(path, QString::fromLocal8Bit(strerror(errno)));

    return ok;
}

void Object::Private::readData()
{
    if (!notifier) // if called from setAttributeCacheEnabled
        return;

    errorString.clear();

    char buffer[QPPS_DEFAULT_PPS_OBJECT_SIZE];

    forever {
        const int count = ::read(notifier->socket(), buffer, sizeof(buffer));
        if (count == -1) {
            errorString = QObject::tr("Unable to read data from %1: %2").arg(path, QString::fromLocal8Bit(strerror(errno)));
            return;
        }

        if (count == 0) {
            // no more data to read, jump out
            break;
        }

        const QList<QByteArray> lines = QByteArray::fromRawData(buffer, count).split('\n');
        if (!parseLines(lines)) {
            // stop in case we failed to parse lines
            return;
        }
    }
}

bool Object::Private::parseLines(const QList<QByteArray> &lines)
{
    Changeset changeset;
    foreach (QByteArray line, lines) {
        // ignore empty lines
        if (line.isEmpty())
            continue;

        // parse [ni] qualifiers; for now we just recognize and remove them to avoid unnecessary parsing errors
        if (line.startsWith('[')) {
            const int closingBrace = line.indexOf(']');
            if (closingBrace == -1) {
                errorString = QObject::tr("Invalid format (no closing brace ']') from %1: '%2'")
                                        .arg(path, QString::fromLocal8Bit(line));
                return false;
            }
            line.remove(0, closingBrace + 1);
        }

        // line tells us which object the following attribute changes belong to
        if (line.startsWith('@')) // object name ### we could compare it to the expected one
            continue;

        if (line.startsWith('-')) { // attribute removed
            const QString name = QString::fromLocal8Bit(line.mid(1));

            if (cache)
                cache->remove(name);

            changeset.removals += name;
        } else { // attribute added or modified
            const int firstColon = line.indexOf(':');
            const int secondColon = line.indexOf(':', firstColon + 1);
            if (firstColon == -1 || secondColon == -1) { // bad format
                errorString = QObject::tr("Invalid format (no '::' encountered) from %1: '%2'")
                                        .arg(path, QString::fromLocal8Bit(line));
                return false;
            }

            const QString name = QString::fromLocal8Bit(line.left(firstColon));
            const QByteArray encoding = line.mid(firstColon + 1, secondColon - firstColon - 1);
            const QByteArray value = line.mid(secondColon + 1);

            Variant variant(value, encoding);

            if (cache)
                cache->insert(name, variant);

            changeset.assignments.insert(name, variant);
        }
    }

    //emit single attribute notifications
    for (QMap<QString, Variant>::ConstIterator assignIt = changeset.assignments.constBegin();
         assignIt != changeset.assignments.constEnd();
         ++assignIt) {
        emit q()->attributeChanged(assignIt.key(), assignIt.value());
    }

    Q_FOREACH(const QString &removal, changeset.removals)
        emit q()->attributeRemoved(removal);

    emit q()->attributesChanged(changeset);
    return true;
}

/**
 * @short Build up path from object path and modifier(s)
 *
 * examples: "/pps/foo", "delta" -> "/pps/foo?delta"
 *           "/pps/foo?nopersist", "delta" -> "/pps/foo?nopersist,delta"
 * and even - this corrects a mistake in the first argument:
 *           "/pps/foo?nopersist?wait", "delta" -> "/pps/foo?nopersist,wait,delta"
 */
static QString buildPath(const QString &objectPath, const QString &modifier)
{
    QString ret;
    QStringList parts = objectPath.split(QLatin1Char('?'));
    // an empty path is invalid, keep it empty and invalid
    if (parts.empty())
        return ret;

    parts.append(modifier);
    ret = parts.takeFirst() + QLatin1Char('?');
    ret += parts.join(QStringLiteral(","));
    return ret;
}

Object::Object(const QString &path, PublicationMode mode, bool create, QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->path = path;

    const QString openPath = buildPath(path, QStringLiteral("delta"));
    // the 3rd argument to open() is mandatory with O_CREAT, according to POSIX...
    const int perm = S_IRUSR | S_IWUSR | S_IRGRP; // TODO configurable?
    const int fd = ::open(openPath.toLocal8Bit(), openFlags(mode, create), perm);
    if (fd == -1) {
        d->errorString = QObject::tr("Unable to open %1: %2").arg(d->path, QString::fromLocal8Bit(strerror(errno)));
        return;
    }

    d->notifier = new QSocketNotifier(fd, QSocketNotifier::Read, d);
    connect(d->notifier, SIGNAL(activated(int)), d, SLOT(readData()));
}

Object::~Object()
{
    if (d->notifier)
        ::close(d->notifier->socket());
}

bool Object::attributeCacheEnabled() const
{
    return d->cache != 0;
}

void Object::setAttributeCacheEnabled(bool cacheEnabled)
{
    if (cacheEnabled == (d->cache != 0))
        return;

    if (cacheEnabled) {
        d->cache = new QHash<QString, Variant>();

        // force initial update, so that we have it as soon as this method returns
        d->readData();
    } else {
        delete d->cache;
        d->cache = 0;
    }
}

bool Object::isValid() const
{
    return d->notifier != 0;
}

QString Object::errorString() const
{
    return d->errorString;
}

QString Object::path() const
{
    return d->path;
}

Variant Object::attribute(const QString &name, const Variant &defaultValue) const
{
    if (!d->cache)
        return defaultValue;

    if (!d->cache->contains(name))
        return defaultValue;

    return d->cache->value(name);
}

QStringList Object::attributeNames() const
{
    if (!d->cache)
        return QStringList();

    return d->cache->keys();
}

bool Object::setAttribute(const QString &name, const Variant &value)
{
    Changeset changeset;
    changeset.assignments.insert(name, value);
    return setAttributes(changeset);
}

bool Object::setAttributes(const Changeset &changeset)
{
    d->errorString.clear();
    if (!d->notifier) {
        d->errorString = tr("Object not valid");
        return false;
    }

    QByteArray data;

    QMap<QString, Variant>::ConstIterator it = changeset.assignments.constBegin();
    for (; it != changeset.assignments.constEnd(); ++it) {
        if (d->cache)
            d->cache->insert(it.key(), it.value());
         data += it.key().toLocal8Bit() + ':' + it.value().encoding() + ':' + it.value().value() + '\n';
    }

    Q_FOREACH (const QString &removedAttr, changeset.removals) {
        if (d->cache)
            d->cache->remove(removedAttr);
        data += '-' + removedAttr.toLocal8Bit() + '\n';
    }

    return d->writeChanges(data);
}

bool Object::removeAttribute(const QString &name)
{
    Changeset changeset;
    changeset.removals += name;
    return setAttributes(changeset);
}
