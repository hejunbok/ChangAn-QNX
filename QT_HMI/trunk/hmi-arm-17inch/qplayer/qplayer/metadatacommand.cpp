#include "metadatacommand.h"

#include <QDebug>

namespace QPlayer {

MetadataCommand::MetadataCommand(int mediaSourceId, QString mediaNodeId)
    : m_mediaSourceId(mediaSourceId)
    , m_mediaNodeId(mediaNodeId)
{
    connect(this, &MetadataCommand::complete, this, &MetadataCommand::deleteLater);
    connect(this, &MetadataCommand::error, this, &MetadataCommand::deleteLater);
}

MetadataCommand::~MetadataCommand()
{

}

int MetadataCommand::mediaSourceId() const
{
    return m_mediaSourceId;
}

QString MetadataCommand::mediaNodeId() const
{
    return m_mediaNodeId;
}

}
