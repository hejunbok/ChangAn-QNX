#ifndef QTQNXCAR2_APPSTATUS_H
#define QTQNXCAR2_APPSTATUS_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/navigation/lampstatus PPS object
 */
class QTQNXCAR2_EXPORT AppStatus : public QObject
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
    Q_PROPERTY(int getAppStatus READ getAppStatus WRITE setAppStatus NOTIFY appStatusChanged)

    /// Whether air conditioning is enabled
    Q_PROPERTY(int getCarplayStatus READ getCarplayStatus WRITE setCarplayStatus NOTIFY carplayStatusChanged)

     /// Whether turn left event occured
    Q_PROPERTY(int getMlinkStatus READ getMlinkStatus WRITE setMlinkStatus NOTIFY mlinkStatusChanged)


public:
    /**
     * Creates a new LampStatus object.
     *
     * @param parent The parent object.
     */
    explicit AppStatus(QObject *parent = 0);

    int getStateByString(QString state);
    int getAppStatus() const;
    Q_INVOKABLE void setAppStatus(int appStatus);
    int getCarplayStatus() const;
    Q_INVOKABLE void setCarplayStatus(int carplayStatus);
    int getMlinkStatus() const;
    Q_INVOKABLE void setMlinkStatus(int mlinkStatus);

Q_SIGNALS:
    void appStatusChanged(int);
    void carplayStatusChanged(int);
    void mlinkStatusChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
