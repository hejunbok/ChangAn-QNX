#ifndef NAVIGATIONSETTINGSMODELS_P_H
#define NAVIGATIONSETTINGSMODELS_P_H

#include "SqlQueryModel_p.h"

#include "NavigationSettingsModel.h"

#include <qpps/object.h>

namespace QnxCar {

class NavigationSettingsModel::Private : public SqlQueryModel::Private
{
public:
    Private();

    int m_activeProfileId;
};

}

#endif // NAVIGATIONSETTINGSMODELS_P_H
