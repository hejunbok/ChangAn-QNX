#ifndef QTQNXCAR2_CONTROLCOMMAND_H
#define QTQNXCAR2_CONTROLCOMMAND_H

#include <QObject>

#include "qtqnxcar2_export.h"

class QJsonDocument;

namespace QPps {
    class Object;
    class Variant;
}

namespace QnxCar {

/**
 * Implements the command/response protocol used by control objects in QNXCar.
 *
 * When start() is called, the (msg, id, dat) triple is written to the control object.
 * The command object listens to changes from the control object and
 * emits finished() once a response with matching @c id is received.
 * ControlCommand objects auto-delete themselves, so there is no need to delete them manually.
 *
 * When creating a command object, setting the message via setMessage() is mandatory.
 * The @c id is auto-assigned by default (process-global increasing counter)
 *
 * Example:
 *
 * @code
 * //zoom navigation map
 *
 * //Create command (must be on the heap, object will delete itself!)
 * ControlCommand *command = new ControlCommand(navigationControlObject, this);
 *
 * //set msg and dat:
 * command->setMessage(QStringLiteral("zoomMap");
 * QJsonObject dat;
 * dat.insert(QStringLiteral("scale"), 2);
 * command->setDat(QJsonDocument(dat));
 *
 * //connect finished() signal
 * connect(command, finished(QnxCar::ControlCommand*),
 *         this, SLOT(zoomFinished(QnxCar::ControlCommand*)));
 * //start execution
 * command->start();
 *
 * @endcode
 *
 * Add a slot like @c zoomFinished(QnxCar::ControlCommand*) to receive the response:
 *
 * @code
 * void MyClass::zoomFinished(QnxCar::ControlCommand *command)
 * {
 *     if (command->hasError()) {
 *         const QString errorString = command->errorString();
 *         //report error here
 *         return;
 *     }
 *
 *     const QJsonDocument responseDat = command->response();
 *     //handle response...
 * }
 * @endcode
 *
 */
class QTQNXCAR2_EXPORT ControlCommand : public QObject
{
    Q_OBJECT
public:
    explicit ControlCommand(QPps::Object *object, QObject *parent = 0);

    /**
     * The message ("msg" attribute) of this command
     */
    QString message() const;

    /**
     * Sets the message ("msg" attribute)
     */
    void setMessage(const QString &message);

    /**
     * Unique ID identifying this command. An process-global increasing counter by default.
     */
    QString id() const;

    /**
     * Sets a custom command ID. Use this to override the auto-generated ID.
     */
    void setId(const QString &id);

    /**
     * The "dat" attribute content.
     */
    QPps::Variant dat() const;

    /**
     * Sets the "dat" attribute. Must be Json.
     */
    void setDat(const QPps::Variant &dat);

    /**
     * Error code returned by the control object. 0 indicates no error.
     */
    int errorCode() const;

    /**
     * Convenience function, returns @c true exactly if errorCode() is not 0
     */
    bool hasError() const;

    /**
     * Error string received from the control object
     */
    QString errorString() const;

    /**
     * Response ("dat") received from the control object
     */
    QJsonDocument response() const;

    /**
     * Response ("dat") received from the control object not in JSON format
     */
    QPps::Variant datResponse() const;

Q_SIGNALS:
    /**
     * Emitted when a response from the control object was received
     *
     * @param command A pointer to the emitting object ("this")
     */
    void finished(QnxCar::ControlCommand *command);

public Q_SLOTS:
    /**
     * Start this command, i.e. writes the msg/id/dat triple to the control
     * object and waits for the response (notified by finished()).
     * Nothing will happen unless start() is called!
     */
    void start();

private:
    class Private;
    Private *const d;
};

} // namespace QnxCar

#endif
