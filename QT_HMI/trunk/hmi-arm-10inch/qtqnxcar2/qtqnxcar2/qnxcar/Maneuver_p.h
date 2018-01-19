#ifndef MANEUVER_P_H
#define MANEUVER_P_H

#include "ManeuversModel.h"

namespace QnxCar {

struct QTQNXCAR2_EXPORT Maneuver
{
    Maneuver();
    QString street;
    ManeuversModel::Command command;
    int distance;
};

}

#endif // MANEUVER_P_H
