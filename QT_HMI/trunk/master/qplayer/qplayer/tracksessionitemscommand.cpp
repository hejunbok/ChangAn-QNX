#include "tracksessionitemscommand.h"

namespace QPlayer {

TrackSessionItemsCommand::TrackSessionItemsCommand(int trackSessionId, int limit, int offset)
    : m_trackSessionId(trackSessionId)
    , m_limit(limit)
    , m_offset(offset)
{
    connect(this, &TrackSessionItemsCommand::complete, this, &TrackSessionItemsCommand::deleteLater);
    connect(this, &TrackSessionItemsCommand::error, this, &TrackSessionItemsCommand::deleteLater);
}

uint64_t TrackSessionItemsCommand::trackSessionId() const
{
    return m_trackSessionId;
}

int TrackSessionItemsCommand::limit() const
{
    return m_limit;
}

int TrackSessionItemsCommand::offset() const
{
    return m_offset;
}

}
