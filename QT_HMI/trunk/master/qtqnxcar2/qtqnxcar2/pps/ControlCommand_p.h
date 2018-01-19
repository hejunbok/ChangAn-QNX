#ifndef QTQNXCAR2_CONTROLCOMMAND_P_H
#define QTQNXCAR2_CONTROLCOMMAND_P_H

#include "ControlCommand.h"

#include <qpps/object.h>

#include <QPointer>
#include <QJsonDocument>

namespace QPps {
    struct Changeset;
}

class QnxCar::ControlCommand::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(QPps::Object *object, ControlCommand *qq);
    void emitFinished();

public Q_SLOTS:
    void doStart();
    void objectChanged(const QPps::Changeset &changes);

public:
    ControlCommand *const q;
    QPointer<QPps::Object> object;
    QString message;
    QString id;
    int errorCode;
    QString errorString;
    QJsonDocument response;
    QPps::Variant dat;
    QPps::Variant datResponse;

    static int s_sequenceId;
};

#endif
