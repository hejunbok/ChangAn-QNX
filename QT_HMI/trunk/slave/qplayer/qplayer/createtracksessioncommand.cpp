#include "createtracksessioncommand.h"

namespace QPlayer {

CreateTrackSessionCommand::CreateTrackSessionCommand(int mediaSourceId, QString mediaNodeId, int index, int limit)
    : m_mediaSourceId(mediaSourceId)
    , m_mediaNodeId(mediaNodeId)
    , m_index(index)
    , m_limit(limit)
{
    connect(this, &CreateTrackSessionCommand::complete, this, &CreateTrackSessionCommand::deleteLater);
    connect(this, &CreateTrackSessionCommand::error, this, &CreateTrackSessionCommand::deleteLater);
}

int CreateTrackSessionCommand::mediaSourceId() const
{
    return m_mediaSourceId;
}

QString CreateTrackSessionCommand::mediaNodeId() const
{
    return m_mediaNodeId;
}

int CreateTrackSessionCommand::index() const
{
    return m_index;
}

int CreateTrackSessionCommand::limit() const
{
    return m_limit;
}


}
