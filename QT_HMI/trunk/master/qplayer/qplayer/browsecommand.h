#ifndef BROWSECOMMAND_H
#define BROWSECOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The BrowseCommand class defines the command interface for the @c browse method.
 *
 * Instances of the BrowseCommand are used to pass parameters to the @c browse method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class BrowseCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a BrowseCommand.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     * @param limit The maximum number of nodes to retrieve. Defaults to -1 (all).
     * @param offset The offset at which to begin retrieving records. Defaults to 0.
     */
    explicit BrowseCommand(int mediaSourceId, QString mediaNodeId, int limit = -1, int offset = 0);

    /**
     * @brief Returns the result of the command.
     * @return A QList of MediaNode objects.
     */
    inline QList<MediaNode> result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(QList<MediaNode> result)
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
     * @brief Returns the limit argument value used to create the command.
     * @return The limit argument value.
     */
    int limit() const;

    /**
     * @brief Returns the offset argument value used to create the command.
     * @return The offset argument value.
     */
    int offset() const;

signals:
    /**
     * @brief The complete signal is emitted when the @c browse command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(BrowseCommand *command);

    /**
     * @brief The error signal is emitted when the @c browse command fails. The error message associated
     * with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(BrowseCommand *command);

private:
    int m_mediaSourceId;
    QString m_mediaNodeId;
    int m_limit;
    int m_offset;
    QList<MediaNode> m_result;
};

}

#endif // BROWSECOMMAND_H
