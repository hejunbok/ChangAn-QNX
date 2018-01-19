#ifndef QPPS_SIMULATOR_P_H
#define QPPS_SIMULATOR_P_H

#include <QMap>
#include <QStack>
#include <QVariant>

namespace QPps {

class SimulatorPrivate : public QObject
{
    Q_OBJECT

public:
    SimulatorPrivate(QObject *parent = 0);

    //uses QMap here to get predictable orders when iterating for unit tests.
    //QHash uses a random seed which defines the iteration order.

    // key: the SimulatorPpsObject client   value: the PPS object path
    QMap<QObject*, QString> m_clients;

    // key: the PPS object path   value: the entries of the PPS object
    QMap<QString, QVariantMap> m_ppsObjects;
};

}

#endif
