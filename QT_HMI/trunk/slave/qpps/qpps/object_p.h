#ifndef QPPS_OBJECT_P_H
#define QPPS_OBJECT_P_H

#include "object.h"

#include <QHash>
#include <QSocketNotifier>

namespace QPps
{

class Object::Private : public QObject
{
    Q_OBJECT

public:
    Private(Object *parent);
    ~Private();

    bool writeChanges(const QByteArray &changes);

    /// @return False in case the parsing failed, else True
    bool parseLines(const QList<QByteArray> &lines);

    Object *q() const { return static_cast<Object *>(parent()); }
    QString path;
    QString errorString;
    QSocketNotifier *notifier;
    QHash<QString, Variant> *cache;

public Q_SLOTS:
    void readData();
};

}

#endif
