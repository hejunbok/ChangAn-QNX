#ifndef QPPS_DIRWATCHER_P_H
#define QPPS_DIRWATCHER_P_H

#include "dirwatcher.h"

#include <QSet>

namespace QPps
{

class DirWatcher::Private : public QObject
{
    Q_OBJECT

public:
    Private(DirWatcher *parent);
    ~Private();

    DirWatcher *q() const { return static_cast<DirWatcher *>(parent()); }
    QString path;
    QString errorString;
    QSet<QString> cache;

private Q_SLOTS:
    void doInitialObjectListing();
    void notifyObjectAdded(const QString &objectPath);
};

}

#endif
