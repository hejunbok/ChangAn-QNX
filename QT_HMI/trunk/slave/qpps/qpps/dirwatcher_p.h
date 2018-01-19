#ifndef QPPS_DIRWATCHER_P_H
#define QPPS_DIRWATCHER_P_H

#include "dirwatcher.h"

#include <QSet>
#include <QSocketNotifier>

namespace QPps
{

class DirWatcher::Private : public QObject
{
    Q_OBJECT

public:
    Private(DirWatcher *parent);
    ~Private();

    /// @return False in case the parsing failed, else True
    bool parseLines(const QList<QByteArray> &lines);

    DirWatcher *q() const { return static_cast<DirWatcher *>(parent()); }
    QString path;
    QString errorString;
    QSocketNotifier *notifier;
    QSet<QString> cache;

public Q_SLOTS:
    void readData();
};

}

#endif
