#include "dirwatcher.h"
#include "simulator_dirwatcher_p.h"

#include "simulator.h"

#include <QFileInfo>
#include <QList>
#include <QStringList>
#include <QVariantMap>

using namespace QPps;

DirWatcher::Private::Private(DirWatcher *parent)
    : QObject(parent)
{
}

void DirWatcher::Private::doInitialObjectListing()
{
    // ### very inefficient
    foreach (const QString &objectPath, Simulator::self()->ppsObjects().keys())
        notifyObjectAdded(objectPath);
}

void DirWatcher::Private::notifyObjectAdded(const QString &objectPath)
{
    QFileInfo fi(objectPath);
    if (fi.path() != path || cache.contains(fi.fileName()))
        return;
    cache.insert(fi.fileName());
    emit q()->objectAdded(fi.fileName());
}

DirWatcher::Private::~Private()
{
}

DirWatcher::DirWatcher(const QString &path, QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    // strip trailing slash, etc.
    d->path = QFileInfo(path).path();

    QMetaObject::invokeMethod(d, "doInitialObjectListing", Qt::QueuedConnection);
    connect(Simulator::self(), SIGNAL(objectAdded(QString)),
            d, SLOT(notifyObjectAdded(QString)));
}

DirWatcher::~DirWatcher()
{
}

bool DirWatcher::isValid() const
{
    return d->errorString.isEmpty();
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
