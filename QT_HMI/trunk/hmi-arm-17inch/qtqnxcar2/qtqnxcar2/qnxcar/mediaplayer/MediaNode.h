#ifndef QTQNXCAR2_MEDIAPLAYER_MEDIANODE_H
#define QTQNXCAR2_MEDIAPLAYER_MEDIANODE_H

#include "qtqnxcar2_export.h"

#include <QVariant>

#include "MediaNodeType.h"
#include "Metadata.h"

namespace QnxCar {
namespace MediaPlayer {

class QTQNXCAR2_EXPORT MediaNode
{
public:
    MediaNode();

    QString id;
    int mediaSourceId;
    QString name;
    MediaNodeType::Type type;
    int count;
    Metadata metadata;
    bool ready;

    /**
     * @brief Returns true if this node has the specified extended metadata property.
     * @param name The extended metadata property name.
     * @return True if the node has the specified extended metadata property, False if not.
     */
    bool hasExtendedMetadata(const QString &name);

    /**
     * @brief Adds or sets an extended metadata property and value to this node.
     * @param name The extended metadata property name.
     * @param value The extended metadata value.
     */
    void setExtendedMetadata(const QString &name, const QVariant value);

    /**
     * @brief Gets an extended metadata value.
     * @param name The extended metadata property name.
     * @return A QVariant containing the extended metadata value, or an invalid QVariant if the property does not exist.
     */
    const QVariant getExtendedMetadata(const QString &name) const;

private:
    QHash<QString, QVariant> m_extendedMetadata;

};

}
}

#endif
