#ifndef QTQNXCAR2_VIDEOPLAYER_P_H
#define QTQNXCAR2_VIDEOPLAYER_P_H

#include "VideoPlayer.h"
#include "AudioPlayer_p.h" // Message/Response/ControlObject
#include "RendererMetadata_p.h"

#include <QScopedPointer>
#include <QVariantMap>
#include <QVector>

#include <qpps/changeset.h>
#include <qpps/dirwatcher.h>
#include <qpps/object.h>

class QDebug;

namespace QnxCar {

class VideoPlayer::Private : public QObject
{
    Q_OBJECT

public:
    Private(VideoPlayer *qq);
    virtual ~Private();


    QnxCar::ControlCommand *createControlCommand(const QString &message,
                                                 const QJsonObject &dat = QJsonObject()) const;
    /**
     * @brief create PPS control object to mm-renderer.
     */
    bool createPPSControlObject();

    VideoPlayer *q;

    /// Context name ("name" param in contextCreate())
    QString m_contextName;
    /// Whether this instance has a context open
    bool m_hasContext;

    QPps::Object *m_ppsControlObject;
    QPps::Object *m_ppsInputObject;
    QPps::Object *m_ppsStateObject;
    QPps::Object *m_ppsStatusObject;

    QPps::DirWatcher *m_contextDirWatcher;
    RendererMetadata *m_metadata;

    QString m_windowGroup;
    QRect m_viewPort;

    /**
     * @brief Attaches video output to mm-renderer.
     */
    void attachVideoOutput();

    /**
     * @brief Attaches audio output to mm-renderer.
     */
    void attachAudioOutput();

    /**
     * @brief Detaches video output from mm-renderer.
     */
    void detachVideoOutput();

    /**
     * @brief Detaches audio output from mm-renderer.
     */
    void detachAudioOutput();

private Q_SLOTS:
    /// Called when the renderer/<contextname>/input object changes
    void ppsInputAttributeChanged(const QString &name, const QPps::Variant &attribute);
    /// Called when the renderer/<contextname>/state object changes
    void ppsStateAttributeChanged(const QString &name, const QPps::Variant &attribute);
    /// Called when the renderer/<contextname>/status object changes
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);

    void handleCreateContextFinished(QnxCar::ControlCommand *cmd);
    void handleCloseContextFinished(QnxCar::ControlCommand *cmd);

    void handleVideoOutputAttached(QnxCar::ControlCommand *cmd);
    void handleAudioOutputAttached(QnxCar::ControlCommand *cmd);

    void contextObjectAdded(const QString &object);
    void contextObjectRemoved(const QString &object);

private:
    int m_videoOutputId;
    int m_audioOutputId;

    void initStatusMonitor();
    void destroyStatusMonitor();

};

}

#endif
