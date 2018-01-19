#include "extendedmetadatacommand.h"

namespace QPlayer {

ExtendedMetadataCommand::ExtendedMetadataCommand(int mediaSourceId, QString mediaNodeId, QStringList properties)
    : m_mediaSourceId(mediaSourceId)
    , m_mediaNodeId(mediaNodeId)
    , m_properties(properties)
{
    connect(this, &ExtendedMetadataCommand::complete, this, &ExtendedMetadataCommand::deleteLater);
    connect(this, &ExtendedMetadataCommand::error, this, &ExtendedMetadataCommand::deleteLater);
}

int ExtendedMetadataCommand::mediaSourceId() const
{
    return m_mediaSourceId;
}

QString ExtendedMetadataCommand::mediaNodeId() const
{
    return m_mediaNodeId;
}

QStringList ExtendedMetadataCommand::properties() const
{
    return m_properties;
}

}
