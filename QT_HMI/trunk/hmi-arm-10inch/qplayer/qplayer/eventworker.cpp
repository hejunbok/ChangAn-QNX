#include "eventworker.h"
#include "qplayer.h"
#include "types.h"
#include "typeconverter.h"

#include <QDebug>
#include <QList>

#include "mmplayer/mmplayerclient.h"
#include "mmplayer/types.h"

namespace QPlayer {

EventWorker::EventWorker(QPlayer *qq, mmplayer_hdl_t *handle)
    : q(qq)
    , m_playerHandle(handle)
    , m_quitRequested(false)
{
}

EventWorker::~EventWorker()
{
}

void EventWorker::requestQuit()
{
    m_quitRequested = true;
}

bool EventWorker::getQuitRequested()
{
    return m_quitRequested;
}

void EventWorker::run()
{
    int rc = 0;
    const mmp_event_t *event;

    if (!m_playerHandle) {
        qCritical("%s: No mm-player handle, aborting", Q_FUNC_INFO);
        return;
    }

    while (!getQuitRequested()) {
        rc = mmp_event_wait(m_playerHandle);

        if (rc == 0) {
            event = mmp_event_get(m_playerHandle);

            if (event != NULL) {
                switch(event->type){
                case MMP_EVENT_MEDIASOURCE:
                {
                    if (event->details.mediasource_info.mediasource != NULL) {
                        emit q->mediaSourceChanged((MediaSourceEventType)event->details.mediasource_info.type,
                                                   TypeConverter::convertMediaSource(*(event->details.mediasource_info.mediasource)));
                    } else {
                        qWarning("%s: MMP_EVENT_MEDIASOURCE - Warning - null media source from player", Q_FUNC_INFO);
                    }
                }
                    break;
                case MMP_EVENT_TRACK:
                {
                    Track track;

                    // TODO: Use same conversion for getCurrentTrack

                    track.index = event->details.track.index;
                    track.tsid = event->details.track.tsid;
                    if (event->details.track.media_node != NULL) {
                        track.mediaNode = TypeConverter::convertMediaNode(*(event->details.track.media_node));
                    } else {
                        qWarning("%s: MMP_EVENT_TRACK - Warning - null media_node from player", Q_FUNC_INFO);
                    }
                    if (event->details.track.metadata != NULL) {
                        track.metadata = TypeConverter::convertMetadata(*(event->details.track.metadata));
                    }
                    emit q->trackChanged(track);
                }
                    break;
                case MMP_EVENT_TRACKPOSITION:
                {
                    // T'would spam the log. Use wisely whilst debugging.
                    emit q->trackPositionChanged(event->details.trkpos.position);
                }
                    break;
                case MMP_EVENT_TRACKSESSION:
                {
                    TrackSession trackSession;

                    trackSession.id = event->details.trksession.tsid;
                    trackSession.length = event->details.trksession.length;
                    emit q->trackSessionChanged((TrackSessionEventType)event->details.trksession.type, trackSession);
                }
                    break;
                case MMP_EVENT_STATE:
                {
                    mmp_state_t state;
                    state.status = event->status;
                    state.shuffle_mode = event->shuffle;
                    state.repeat_mode = event->repeat;
                    state.rate = event->rate;

                    emit q->playerStateChanged(TypeConverter::convertPlayerState(state));
                }
                    break;
                case MMP_EVENT_NONE:
                case MMP_EVENT_OTHER:
                case MMP_EVENT_ERROR:
                    qWarning("%s: Event fell through (NONE, OTHER, ERROR), ignoring. Event Type: %d", Q_FUNC_INFO, event->type);
                    break;
                }
            } else {
                qDebug("%s: Received null event, ignoring", Q_FUNC_INFO);
            }
        } else {
            qDebug("%s: Error on wait, ignoring", Q_FUNC_INFO);
        }
    }
}

}
