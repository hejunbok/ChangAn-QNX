#include "MediaNode.h"

namespace QnxCar {
namespace MediaPlayer {

MediaNode::MediaNode()
    : mediaSourceId(-1)
    , type(MediaNodeType::UNKNOWN)
    , ready(true)
{
}

bool MediaNode::hasExtendedMetadata(const QString &name)
{
    return m_extendedMetadata.contains(name);
}

void MediaNode::setExtendedMetadata(const QString &name, const QVariant value)
{
    if(m_extendedMetadata.contains(name)) {
         m_extendedMetadata.find(name).value() = value;
    } else {
        m_extendedMetadata.insert(name, value);
    }
}

const QVariant MediaNode::getExtendedMetadata(const QString &name) const
{
    QVariant value;

    if(m_extendedMetadata.contains(name))
    {
        value = m_extendedMetadata.value(name);
    }

    return value;
}

}
}
