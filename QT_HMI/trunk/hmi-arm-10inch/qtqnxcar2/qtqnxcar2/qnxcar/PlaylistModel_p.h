#ifndef PLAYLISTMODEL_P_H
#define PLAYLISTMODEL_P_H

#include "SqlQueryModel_p.h"

namespace QnxCar {

class PlaylistModel::Private : public SqlQueryModel::Private
{
public:
    explicit Private(const QString &player) : playerName(player) {}

    QString playerName;
};

}

#endif // PLAYLISTMODEL_P_H
