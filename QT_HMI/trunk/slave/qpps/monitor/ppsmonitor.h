#ifndef PPSMONITOR_H
#define PPSMONITOR_H

#include <QObject>

class PpsMonitor : public QObject
{
    Q_OBJECT
public:
    explicit PpsMonitor(QObject *parent = 0);

    void watchRecursive(const QString &dirName);
};

#endif // PPSMONITOR_H
