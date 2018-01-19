#ifndef PLAYER_P_H
#define PLAYER_P_H

#include "qplayer.h"
#include "eventworker.h"
#include "commandworker.h"

#include <mmplayer/mmplayerclient.h>
#include <mmplayer/types.h>

#include <QMutex>
#include <QTime>
#include <QThread>
#include "qpps/dirwatcher.h"

namespace QPlayer {

/**
 * @brief Number of milliseconds to wait for the event or command threads to quit before
 * forcefully terminating them.
 */
static const int THREAD_WAIT_MS = 1500;

class QPlayer::Private : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new QPlayer private implementation instance.
     * @param playerName The mm-player player name to open upon initialization.
     * @param q A pointer to the QPlayer public implementaiton instance.
     */
    explicit Private(const QString playerName, QPlayer *q = 0);
    virtual ~Private();

    /**
     * @brief Connects to the specified player name.
     * @param name The player name.
     * @return
     */
    int open(QString name);

    /**
     * @brief Disconnects from the specified player name.
     * @param name The player name.
     * @return
     */
    int close(QString name);

    /**
     * @brief Starts the event thread, responsible for emitting QPlayer signals.
     * @param handle The mm-player handle acquired upon initialization.
     * @return
     */
    int startEventThread(mmplayer_hdl_t *handle);

    /**
     * @brief Starts the command thread, responsible for executing QPlayer commands.
     * @param handle The mm-player handle acquired upon initialization.
     * @return
     */
    int startCommandThread(mmplayer_hdl_t *handle);

    /**
     * @brief Stops the event thread.
     * @return
     */
    int stopEventThread();

    /**
     * @brief Stops the command thread.
     * @return
     */
    int stopCommandThread();

    QPlayer *const q;

    bool m_playerOpen;
    QString m_playerName;
    mmplayer_hdl_t *m_playerHandle;
    QMutex mmplayerHandlingLock;

    EventWorker *m_eventWorker;
    QThread *m_eventThread;
    CommandWorker *m_commandWorker;
    QThread *m_commandThread;
    QPps::DirWatcher *m_bootMgrDirWatcher;

public Q_SLOTS:
    void bootMgrObjAdded(const QString &object);

};

}

#endif // PLAYER_P_H
