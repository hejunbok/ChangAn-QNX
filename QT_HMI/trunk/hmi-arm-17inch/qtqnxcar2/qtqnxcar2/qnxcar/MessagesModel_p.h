#ifndef MESSAGESMODEL_P_H
#define MESSAGESMODEL_P_H

#include "SqlQueryModel_p.h"

namespace QnxCar {

class MessagesModel::Private : public SqlQueryModel::Private
{
public:
    Private() : messageType(-1) {}

    int messageType;
};

}

#endif // MESSAGESMODEL_P_H
