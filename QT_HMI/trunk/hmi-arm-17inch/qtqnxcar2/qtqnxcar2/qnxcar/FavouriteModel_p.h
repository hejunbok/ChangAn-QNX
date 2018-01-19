#ifndef FAVOURITEMODEL_P_H
#define FAVOURITEMODEL_P_H

#include "SqlQueryModel_p.h"

namespace QnxCar {

class FavouriteModel::Private : public SqlQueryModel::Private
{
public:
    Private() : activeProfileId(-1) {}

    int activeProfileId;
};

}

#endif // FAVOURITEMODEL_P_H
