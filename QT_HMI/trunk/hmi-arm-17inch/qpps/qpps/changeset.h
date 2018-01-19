#ifndef QPPS_CHANGESET_H
#define QPPS_CHANGESET_H

#include "variant.h"

#include <QMap>
#include <QSet>

namespace QPps
{

struct Changeset
{
    QMap<QString, Variant> assignments;
    QSet<QString> removals;
};

}

#endif
