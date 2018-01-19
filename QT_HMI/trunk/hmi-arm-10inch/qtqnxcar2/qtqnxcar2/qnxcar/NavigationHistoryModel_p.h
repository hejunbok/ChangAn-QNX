#ifndef NAVIGATIONHISTORYMODEL_P_H
#define NAVIGATIONHISTORYMODEL_P_H

#include "SqlQueryModel_p.h"

namespace QnxCar {

class NavigationHistoryModel::Private : public SqlQueryModel::Private
{
public:
    Private() : activeProfileId(-1) {}

    int activeProfileId;
};

}

#endif // NAVIGATIONHISTORYMODEL_P_H
