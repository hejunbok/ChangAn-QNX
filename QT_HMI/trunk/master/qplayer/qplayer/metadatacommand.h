#ifndef METADATACOMMAND_H
#define METADATACOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The MetadataCommand class defines the command interface for the @c getMetadata method.
 *
 * Instances of the MetadataCommand are used to pass parameters to the @c getMetadata method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class MetadataCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a MetadataCommand.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     */
    explicit MetadataCommand(int mediaSourceId, QString mediaNodeId);
    ~MetadataCommand();

    /**
     * @brief Returns the result of the command.
     * @return A Metadata object populated with the basic metadata for the specified track.
     */
    inline QPlayer::Metadata result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(QPlayer::Metadata result)
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

signals:
    /**
     * @brief The complete signal is emitted when the @c getMetadata command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(MetadataCommand *command);

    /**
     * @brief The error signal is emitted when the @c getMetadata command fails. The error message associated
     * with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(MetadataCommand *command);

private:
    int m_mediaSourceId;
    QString m_mediaNodeId;
    QPlayer::Metadata m_result;
};

}

#endif // METADATACOMMAND_H
