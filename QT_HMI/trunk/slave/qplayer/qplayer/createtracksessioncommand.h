#ifndef CREATETRACKSESSIONCOMMAND_H
#define CREATETRACKSESSIONCOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The CreateTrackSessionCommand class defines the command interface for the @c createTrackSession method.
 *
 * Instances of the CreateTrackSessionCommand are used to pass parameters to the @c createTrackSession method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class CreateTrackSessionCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a CreateTrackSessionCommand.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     * @param index The index of the track to be selected upon creation. Defaults to 0 (first track).
     * @param limit The maximum number of items to add to the track session. Defaults to -1 (all).
     */
    explicit CreateTrackSessionCommand(int mediaSourceId, QString mediaNodeId, int index = 0, int limit = -1);

    /**
     * @brief Returns the result of the command.
     * @return A TrackSession object describing the current track session information.
     */
    inline TrackSession result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(TrackSession result)
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
     * @brief Returns the index argument value used to create the command.
     * @return The index argument value.
     */
    int index() const;

    /**
     * @brief Returns the limit argument value used to create the command.
     * @return The limit argument value.
     */
    int limit() const;

signals:
    /**
     * @brief The complete signal is emitted when the @c createTrackSession command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(CreateTrackSessionCommand *command);

    /**
     * @brief The error signal is emitted when the @c createTrackSession command fails. The error message associated
     * with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(CreateTrackSessionCommand *command);

private:
    int m_mediaSourceId;
    QString m_mediaNodeId;
    int m_index;
    int m_limit;
    TrackSession m_result;
};

}

#endif // CREATETRACKSESSIONCOMMAND_H
