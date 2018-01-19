#ifndef CALLERIDOBJECT_H
#define CALLERIDOBJECT_H

#include "qtqnxcar2_export.h"

#include <QObject>
#include <QString>
#include <QUrl>

namespace QnxCar {
    struct CallerID;

/**
 * Wrapper for exposing conact data to QML
 *
 */

class QTQNXCAR2_EXPORT CallerIDObject : public QObject
{
    Q_OBJECT
   /**
    * The contacts name
    *
    * @accessors name(), setName()
    */
   Q_PROPERTY(QString name READ name WRITE setName NOTIFY changed)
   /**
    * The contacts number
    *
    * @accessors number(), setNumber()
    */
   Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY changed)
   /**
    * The contacts picture
    *
    * @accessors picture(), setPicture()
    */
   Q_PROPERTY(QString picture READ picture WRITE setPicture NOTIFY changed)
public:
   /**
    * Creates an empty wrapper
    * @param parent the QObject parent
    */
    explicit CallerIDObject(QObject *parent = 0);

    /// Destroys the instance
    ~CallerIDObject();

    /**
     * Sets the callerID data to expose to QML
     *
     * @param callerID the data to expose
     * @sa setCallerID
     */
    void setCallerID(const CallerID &callerID);

    /**
     * Returns the currently exposed callerID data
     *
     * @return the exposed data
     * @sa setCallerID()
     */
    CallerID callerID() const;

    /// @sa #name
    QString name() const;
    /// @sa #name
    void setName(const QString &name);

    /// @sa #number
    QString number() const;
    /// @sa #number
    void setNumber(const QString &number);

    /// @sa #picture
    QString picture() const;
    /// @sa #picture
    void setPicture(const QString &picture);

Q_SIGNALS:
    void changed();

private:
    class Private;
    Private *const d;
};
}

#endif // CALLERIDOBJECT_H
