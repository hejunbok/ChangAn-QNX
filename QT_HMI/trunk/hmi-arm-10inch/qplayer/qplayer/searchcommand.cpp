#include "searchcommand.h"

namespace QPlayer {

SearchCommand::SearchCommand(int mediaSourceId, QString searchTerm, QString filter, int limit, int offset)
    : m_mediaSourceId(mediaSourceId)
    , m_searchTerm(searchTerm)
    , m_filter(filter)
    , m_limit(limit)
    , m_offset(offset)
{
    connect(this, &SearchCommand::complete, this, &SearchCommand::deleteLater);
    connect(this, &SearchCommand::error, this, &SearchCommand::deleteLater);
}

int SearchCommand::mediaSourceId() const
{
    return m_mediaSourceId;
}

QString SearchCommand::searchTerm() const
{
    return m_searchTerm;
}

QString SearchCommand::filter() const
{
    return m_filter;
}

int SearchCommand::limit() const
{
    return m_limit;
}

int SearchCommand::offset() const
{
    return m_offset;
}

}
