#ifndef QTQNXCAR2_CAMERACONTROL_H
#define QTQNXCAR2_CAMERACONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/navigation/lampstatus PPS object
 */
class QTQNXCAR2_EXPORT CameraControl : public QObject
{
    Q_OBJECT


    /**
     * The setting for the left fan in the range from 0 to 5.
     *
     * Meaning:
     * - 0: Off
     * - 1: Red
     * - 2: Blue
     * - 3: Yellow
     * - 4: Pink
     * - 5: Green
     */
    Q_PROPERTY(QString capture READ capture WRITE setCapture NOTIFY captureChanged)

    /// Whether air conditioning is enabled
    Q_PROPERTY(QString direction READ direction WRITE setDirection NOTIFY directionChanged)

     /// Whether turn left event occured
    Q_PROPERTY(QString left_port READ left_port WRITE setLeftPort NOTIFY leftportChanged)

    /// Whether turn left event occured
   Q_PROPERTY(QString right_port READ right_port WRITE setRightPort NOTIFY rightportChanged)


public:
    /**
     * Creates a new LampStatus object.
     *
     * @param parent The parent object.
     */
    explicit CameraControl(QObject *parent = 0);

    QString capture() const;
    Q_INVOKABLE void setCapture(QString capture);
    QString direction() const;
    Q_INVOKABLE void setDirection(QString direction);
    QString left_port() const;
    Q_INVOKABLE void setLeftPort(QString left_port);
    QString right_port() const;
    Q_INVOKABLE void setRightPort(QString right_port);

Q_SIGNALS:
    void captureChanged(QString);
    void directionChanged(QString);
    void leftportChanged(QString);
    void rightportChanged(QString);

private:
    class Private;
    Private* const d;
};

}

#endif
