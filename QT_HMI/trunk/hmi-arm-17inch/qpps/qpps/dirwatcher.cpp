#include "dirwatcher.h"
#include "dirwatcher_p.h"

#include <QList>
#include <QStringList>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

using namespace QPps;

static const int QPPS_DEFAULT_PPS_OBJECT_SIZE = 1 << 16;

DirWatcher::Private::Private(DirWatcher *parent)
    : QObject(parent)
    , notifier(0)
{
}

DirWatcher::Private::~Private()
{
}

void DirWatcher::Private::readData()
{
    if (!notifier)
        return;

    errorString.clear();

    char buffer[QPPS_DEFAULT_PPS_OBJECT_SIZE];

    forever {
        const int count = ::read(notifier->socket(), buffer, sizeof(buffer));
        if (count == -1) {
            errorString = QObject::tr("Unable to read data from %1: %2").arg(path, QString::fromLocal8Bit(strerror(errno)));
            return;
        } else if (count == 0) {
            // end of data
            break;
        }

        const QList<QByteArray> lines = QByteArray::fromRawData(buffer, count).split('\n');
        if (!parseLines(lines)) {
            // stop in case we failed to parse lines
            return;
        }
    }
}

bool DirWatcher::Private::parseLines(const QList<QByteArray> &lines)
{
    foreach (QByteArray line, lines) {
        // ignore empty lines
        if (line.isEmpty())
            continue;

        // parse qualifiers; for now we just recognize and remove them to avoid unnecessary parsing errors
        if (line.startsWith('[')) {
            const int closingBrace = line.indexOf(']');
            if (closingBrace == -1) {
                errorString = QObject::tr("Invalid format (no closing brace ']') from %1: '%2'")
                                        .arg(path, QString::fromLocal8Bit(line));
                return false;
            }
            line.remove(0, closingBrace + 1);
        }

        if (line.startsWith('-')) { // object removed, "-@<objectName>"
            const QString name = QString::fromLocal8Bit(line.mid(2));

            cache.remove(name);

            emit q()->objectRemoved(name);
        } else if (line.startsWith('+') || line.startsWith('@')) { // object added, either "+@..." or "@..."
            const bool hasPlus = line.startsWith('+');
            const QString name = QString::fromLocal8Bit(line.mid(hasPlus ? 2 : 1));

            // There is a bug in PPS: when n objects already exist in the directory, PPS reports n objects n times
            // for n^2 updates total. While we can't do much about the performance issue, we can filter out the
            // redundant updates.
            if (!hasPlus && cache.contains(name)) // duplicate in initial listing
                continue;

            cache.insert(name);

            emit q()->objectAdded(name);
        } else {
            errorString = QObject::tr("Invalid line from %1: '%2'")
                                    .arg(path, QString::fromLocal8Bit(line));
            return false;
        }
    }
    return true;
}

DirWatcher::DirWatcher(const QString &path, QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->path = path;

    const QString openPath = path + QStringLiteral("/.all?deltadir");
    const int fd = ::open(openPath.toLocal8Bit(), O_RDONLY);
    if (fd == -1) {
        d->errorString = QObject::tr("Unable to open %1: %2").arg(d->path, QString::fromLocal8Bit(strerror(errno)));
        return;
    }

    d->notifier = new QSocketNotifier(fd, QSocketNotifier::Read, d);
    d->notifier->setParent(d);
    connect(d->notifier, SIGNAL(activated(int)), d, SLOT(readData()));
}

DirWatcher::~DirWatcher()
{
    if (d->notifier)
        ::close(d->notifier->socket());
}

bool DirWatcher::isValid() const
{
    return d->notifier != 0;
}

QString DirWatcher::errorString() const
{
    return d->errorString;
}

QString DirWatcher::path() const
{
    return d->path;
}

QStringList DirWatcher::objectNames() const
{
    return d->cache.toList();
}
