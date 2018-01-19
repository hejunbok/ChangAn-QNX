#include "browsecommand.h"

namespace QPlayer {

BrowseCommand::BrowseCommand(int mediaSourceId, QString mediaNodeId, int limit, int offset)
    : m_mediaSourceId(mediaSourceId)
    , m_mediaNodeId(mediaNodeId)
    , m_limit(limit)
    , m_offset(offset)
{
    connect(this, &BrowseCommand::complete, this, &BrowseCommand::deleteLater);
    connect(this, &BrowseCommand::error, this, &BrowseCommand::deleteLater);
}

int BrowseCommand::mediaSourceId() const
{
    return m_mediaSourceId;
}

QString BrowseCommand::mediaNodeId() const
{
    return m_mediaNodeId;
}

int BrowseCommand::limit() const
{
    return m_limit;
}

int BrowseCommand::offset() const
{
    return m_offset;
}

}
