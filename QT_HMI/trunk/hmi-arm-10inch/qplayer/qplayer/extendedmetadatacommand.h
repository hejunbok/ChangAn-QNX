#ifndef EXTENDEDMETADATACOMMAND_H
#define EXTENDEDMETADATACOMMAND_H

#include <QHash>
#include <QStringList>
#include <QVariant>

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The ExtendedMetadataCommand class defines the command interface for the @c getExtendedMetadata method.
 *
 * Instances of the ExtendedMetadataCommand are used to pass parameters to the getExtendedMetadata method. The
 * complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class ExtendedMetadataCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of an ExtendedMetadataCommand.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     * @param properties A QStringList which contains the list of extended metadata properties to retrieve.
     */
    explicit ExtendedMetadataCommand(int mediaSourceId, QString mediaNodeId, QStringList properties);

    /**
     * @brief Returns the result of the command.
     * @return A QHash containing a series of key/value pairs, where the keys are the requested extended metadata
     * properties. The associated QVariant value is either populated with the extended metadata value, or is an
     * invalid QVariant if the property does not exist for the node.
     */
    inline QHash<QString, QVariant> result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(const QHash<QString, QVariant> result)
    {
        m_result = result;
    }

    /**
     * @brief Returns the mediaSourceId argument value used to create the command.
     * @return The mediaSourceId argument value.
     */
    int mediaSourceId() const;

    /**
     * @brief Returns the mediaNodeId argument value used to create the command.
     * @return The mediaNodeId argument value.
     */
    QString mediaNodeId() const;

    /**
     * @brief Returns the properties argument value used to create the command.
     * @return The properties argument value.
     */
    QStringList properties() const;

signals:
    /**
     * @brief The complete signal is emitted when the @c getExtendedMetadata command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(ExtendedMetadataCommand *command);

    /**
     * @brief The error signal is emitted when the @c getExtendedMetadata command fails. The error message associated
     * with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(ExtendedMetadataCommand *command);

private:
    int m_mediaSourceId;
    QString m_mediaNodeId;
    QStringList m_properties;
    QHash<QString, QVariant> m_result;
};

}

#endif // EXTENDEDMETADATACOMMAND_H
