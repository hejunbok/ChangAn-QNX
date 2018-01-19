#ifndef QPLAYERSTATECOMMAND_H
#define QPLAYERSTATECOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The PlayerStateCommand class defines the command interface for the @c getPlayerState method.
 *
 * Instances of the PlayerStateCommand are used to pass parameters to the @c getPlayerState method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class PlayerStateCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a PlayerStateCommand.
     */
    explicit PlayerStateCommand();

    /**
     * @brief Returns the result of the command.
     * @return A PlayerState object describing the current state of the opened player.
     */
    inline PlayerState result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(PlayerState result)
    {
        m_result = result;
    }

signals:
    /**
     * @brief The complete signal is emitted when the @c getPlayerState command completes. The result of the command
     * can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(PlayerStateCommand *command);

    /**
     * @brief The error signal is emitted when the @c getPlayerState command fails. The error message associated
     * with the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(PlayerStateCommand *command);

private:
    PlayerState m_result;

};

}

#endif // QPLAYERSTATECOMMAND_H
