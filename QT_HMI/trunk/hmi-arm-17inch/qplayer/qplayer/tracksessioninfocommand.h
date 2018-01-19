#ifndef TRACKSESSIONINFOCOMMAND_H
#define TRACKSESSIONINFOCOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The TrackSessionInfoCommand class defines the command interface for the @c getCurrentTrackSessionInfo method.
 *
 * Instances of the TrackSessionInfoCommand are used to pass parameters to the @c getCurrentTrackSessionInfo method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class TrackSessionInfoCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a TrackSessionInfoCommand.
     */
    explicit TrackSessionInfoCommand();

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

signals:
    /**
     * @brief The complete signal is emitted when the @c getCurrentTrackSessionInfo command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(TrackSessionInfoCommand *command);

    /**
     * @brief The error signal is emitted when the @c getCurrentTrackSessionInfo command fails, or if there is no current
     * track session. The error message associated with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(TrackSessionInfoCommand *command);

private:
    TrackSession m_result;
};

}

#endif // TRACKSESSIONINFOCOMMAND_H
