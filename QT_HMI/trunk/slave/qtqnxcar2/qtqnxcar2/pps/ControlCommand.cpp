#include "ControlCommand.h"
#include "ControlCommand_p.h"

#include <qpps/changeset.h>

#include <QDebug>
#include <QJsonDocument>
#include <QRegExp>

using namespace QnxCar;

static const bool DEBUG_ENABLED = false;

int QnxCar::ControlCommand::Private::s_sequenceId = 0;

ControlCommand::Private::Private(QPps::Object *o, ControlCommand *qq)
    : QObject(qq)
    , q(qq)
    , object(o)
    , id(QString::number(s_sequenceId++))
    , errorCode(0)
{
    connect(object, SIGNAL(attributesChanged(QPps::Changeset)), this, SLOT(objectChanged(QPps::Changeset)));
}

void ControlCommand::Private::doStart()
{
    if (!object) {
        errorCode = -1;
        errorString = tr("Control PPS object deleted");
        emitFinished();
        return;
    }

    QPps::Changeset cmd;
    cmd.assignments.insert(QStringLiteral("msg"), message);
    cmd.assignments.insert(QStringLiteral("id"), id);
    cmd.assignments.insert(QStringLiteral("dat"), dat);

    if (DEBUG_ENABLED) {
        qDebug() << "Command:" << cmd.assignments;
    }

    if (!object->setAttributes(cmd)) {
        errorCode = -1;
        errorString = tr("Could not send command to %1: %2").arg(object->path(), object->errorString());
        emitFinished();
    }
}

void ControlCommand::Private::emitFinished()
{
    emit q->finished(q);
    q->deleteLater();
}

void ControlCommand::Private::objectChanged(const QPps::Changeset &changes)
{
    const QMap<QString,QPps::Variant>::ConstIterator it = changes.assignments.constFind(QStringLiteral("id"));

    if (it == changes.assignments.constEnd()) {
        return;
    }

    const QString receivedId = it->toString();

    if (receivedId.isEmpty() || id != receivedId) {
        return;
    }

    QPps::Variant err = changes.assignments.value(QStringLiteral("err"));
    bool errorSet = !err.value().isEmpty();

    if (errorSet) {
        errorCode = -1;
        errorString = changes.assignments.value(QStringLiteral("errstr")).toString();
    }

    if (changes.assignments.contains(QStringLiteral("dat"))) {
        response = changes.assignments.value(QStringLiteral("dat")).toJson(); // This is wrong, becsaue not always response will be in JSON format
        datResponse = changes.assignments.value(QStringLiteral("dat")); // Workaround fto mitigate prev comment
    }

    if (DEBUG_ENABLED) {
        qDebug() << "Response:" << changes.assignments;
    }

    emitFinished();
}

ControlCommand::ControlCommand(QPps::Object *object, QObject *parent)
    : QObject(parent)
    , d(new Private(object, this))
{
}

void ControlCommand::start()
{
    QMetaObject::invokeMethod(d, "doStart", Qt::QueuedConnection);
}

QString ControlCommand::message() const
{
    return d->message;
}

void ControlCommand::setMessage(const QString &message)
{
    d->message = message;
}

QString ControlCommand::id() const
{
    return d->id;
}

void ControlCommand::setId(const QString &id)
{
    d->id = id;
}

QJsonDocument ControlCommand::response() const
{
    return d->response;
}

QPps::Variant ControlCommand::datResponse() const
{
    return d->datResponse;
}

QPps::Variant ControlCommand::dat() const
{
    return d->dat;
}

void ControlCommand::setDat(const QPps::Variant &dat)
{
    //Q_ASSERT(dat.encoding() == "json"); // Commented out becasue not always payload of massage will be JSON onject
    d->dat = dat;
}

int ControlCommand::errorCode() const
{
    return d->errorCode;
}

bool ControlCommand::hasError() const
{
    return d->errorCode != 0;
}

QString ControlCommand::errorString() const
{
    return d->errorString;
}
