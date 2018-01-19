#ifndef QTQNXCAR2_AUDIOPLAYER_P_H
#define QTQNXCAR2_AUDIOPLAYER_P_H

#include "AudioPlayer.h"

#include <QJsonObject>
#include <QVariantMap>
#include <QVector>

#include "pps/ControlCommand.h"

#include <qpps/changeset.h>
#include <qpps/object.h>

namespace QnxCar {

class JsonObjectBuilder
{
public:
    explicit JsonObjectBuilder(const QJsonObject &json = QJsonObject());

    JsonObjectBuilder& attribute(const QString &name, const QJsonValue &value);

    QJsonObject toJson() const { return m_json; }

private:
    QJsonObject m_json;
};

class AudioPlayer::Private : public QObject
{
    Q_OBJECT

public:
    Private(AudioPlayer *qq);

    /// Create player control command (requires open context)
    QnxCar::ControlCommand *createControlCommand(const QString &message,
                                                 const QJsonObject &dat = QJsonObject()) const;

    AudioPlayer *q;

    /// Player name
    QString m_playerContext;
    /// Whether this instance has a context open
    bool m_hasContext;

    QPps::Object *m_ppsControlObject;
    QPps::Object *m_ppsStatusObject;

    RepeatMode m_repeatMode;
    ShuffleMode m_shuffleMode;

public Q_SLOTS:
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void ppsStatusAttributeRemoved(const QString &name);

    void handleCreateContextFinished(QnxCar::ControlCommand *cmd);
    void handleSetReadModeFinished(QnxCar::ControlCommand *cmd);

private:
    void init();
    void initStatusMonitor();
};

}

#endif
