#ifndef MEDIASOURCESCOMMAND_H
#define MEDIASOURCESCOMMAND_H

#include "basecommand.h"
#include "types.h"

namespace QPlayer {

/**
 * @brief The MediaSourcesCommand class defines the command interface for the @c getMediaSources method.
 *
 * Instances of the MediaSourcesCommand are used to pass parameters to the @c getMediaSources method.
 * The @c complete signal is emitted when the command is complete and the result is available.
 *
 * Instances of this class will delete themselves once all error and complete signal handlers have been invoked.
 */
class MediaSourcesCommand : public BaseCommand
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new instance of a MediaSourcesCommand.
     */
    explicit MediaSourcesCommand();

    /**
     * @brief Returns the result of the command.
     * @return A QList populated with the media sources currently connected to the device.
     */
    inline QList<MediaSource> result()
    {
        return m_result;
    }

    /**
     * @brief Sets the command result.
     * @param result The command result.
     */
    inline void setResult(QList<MediaSource> result)
    {
        m_result = result;
    }

signals:
    /**
     * @brief The complete signal is emitted when the @c getMediaSources command completes. The result of the
     * command can then be retrieved via the @c result method.
     * @param command A pointer to the command which triggered the result.
     * @sa result
     */
    void complete(MediaSourcesCommand *command);

    /**
     * @brief The error signal is emitted when the @c getMediaSources command fails. The error message associated with
     * the error can be retrieved via the @c getErrorMessage method.
     * @param command A pointer to the command which triggered the error.
     * @sa errorMessage
     */
    void error(MediaSourcesCommand *command);

private:
    QList<MediaSource> m_result;

};

}

#endif // MEDIASOURCESCOMMAND_H
