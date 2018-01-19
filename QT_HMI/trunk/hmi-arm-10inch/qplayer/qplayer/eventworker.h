#ifndef EVENTS_H
#define EVENTS_H

#include <QObject>
#include <QList>
#include <QString>
#include <QUrl>
#include <QThread>
#include <QMutex>
#include <stdint.h>

#include "qplayer.h"
#include "types.h"

#include <mmplayer/mmplayerclient.h>
#include <mmplayer/event.h>
#include <mmplayer/types.h>

#include "qplayer_export.h"


namespace QPlayer {

class QPLAYER_EXPORT EventWorker : public QObject
{
    Q_OBJECT

public:
    explicit EventWorker(QPlayer *qq, mmplayer_hdl_t *handle);
    ~EventWorker();

    Q_INVOKABLE void run();

    void requestQuit();
    bool getQuitRequested();

private:
    QPlayer *const q;
    mmplayer_hdl_t *m_playerHandle;
    bool m_quitRequested;

private:
};

}
#endif // EVENTS_H
