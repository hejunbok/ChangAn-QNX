#ifndef CURRENTTRACKPOSITIONCOMMAND_H
#define CURRENTTRACKPOSITIONCOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The CurrentTrackPositionCommand class defines the command interface for the @c getCurrentTrackPosition method.
 *
 * Instances of the CurrentTrackPositionCommand are used to pass parameters to the @c getCurrentTrackPosition method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class CurrentTrackPositionCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a CurrentTrackPositionCommand.
     */
    explicit CurrentTrackPositionCommand();

    /**
     * @brief Returns the result of the command.
     * @return The current playback position of the current track, in milliseconds.
     */
    inline int result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(int result)
    {
        m_result = result;
    }

signals:
    /**
     * @brief The complete signal is emitted when the @c getCurrentTrackPosition command completes. The result of the
     * command can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(CurrentTrackPositionCommand *command);

    /**
     * @brief The error signal is emitted when the @c getCurrentTrackPosition command fails, or if there is no current
     * track. The error message associated with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(CurrentTrackPositionCommand *command);

private:
    int m_result;

};

}

#endif // CURRENTTRACKPOSITIONCOMMAND_H
