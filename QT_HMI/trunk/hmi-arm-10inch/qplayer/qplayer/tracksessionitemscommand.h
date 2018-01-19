#ifndef TRACKSESSIONITEMSCOMMAND_H
#define TRACKSESSIONITEMSCOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The TrackSessionItemsCommand class defines the command interface for the @c getTrackSessionItems method.
 *
 * Instances of the TrackSessionItemsCommand are used to pass parameters to the @c getTrackSessionItems method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class TrackSessionItemsCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a TrackSessionItemsCommand.
     * @param trackSessionId The ID of the track session from which to retrieve items.
     * @param limit The maximum number of items to add to the track session. Defaults to -1 (all).
     * @param offset The offset at which to begin retrieving records. Defaults to 0.
     */
    explicit TrackSessionItemsCommand(int trackSessionId, int limit = -1, int offset = 0);

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
     * @brief Returns the trackSessionId argument value used to create the command.
     * @return The trackSessionId argument value.
     */
    uint64_t trackSessionId() const;

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
    void complete(TrackSessionItemsCommand *command);
    void error(TrackSessionItemsCommand *command);

private:
    uint64_t m_trackSessionId;
    int m_limit;
    int m_offset;
    QList<MediaNode> m_result;
};

}

#endif // TRACKSESSIONITEMSCOMMAND_H
