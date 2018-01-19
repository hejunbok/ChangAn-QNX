#ifndef CURRENTTRACKCOMMAND_H
#define CURRENTTRACKCOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The CurrentTrackCommand class defines the command interface for the @c getCurrentTrack method.
 *
 * Instances of the CurrentTrackCommand are used to pass parameters to the getCurrentTrack method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class CurrentTrackCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a CurrentTrackCommand.
     */
    explicit CurrentTrackCommand();

    /**
     * @brief Returns the result of the command.
     * @return A Track object describing the current track information, including metadata.
     */
    inline Track result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(Track result)
    {
        m_result = result;
    }

signals:
    /**
     * @brief The complete signal is emitted when the @c getCurrentTrack command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(CurrentTrackCommand *command);

    /**
     * @brief The error signal is emitted when the @c getCurrentTrack command fails, or if there is no current
     * track. The error message associated with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(CurrentTrackCommand *command);

private:
    Track m_result;

};

}

#endif // CURRENTTRACKCOMMAND_H
