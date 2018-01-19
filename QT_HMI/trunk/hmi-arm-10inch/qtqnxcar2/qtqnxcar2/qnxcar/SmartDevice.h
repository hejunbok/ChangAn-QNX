#ifndef QTQNXCAR2_SMART_DEVICE_H
#define QTQNXCAR2_SMART_DEVICE_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/hvac PPS object
 */
class QTQNXCAR2_EXPORT SmartDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int heartRate READ heartRate WRITE setHeartRate NOTIFY heartRateChanged)
    Q_PROPERTY(int pulse READ pulse WRITE setPulset NOTIFY pulseChanged)
    Q_PROPERTY(int sleepQuality READ sleepQuality WRITE setSleepQuality NOTIFY sleepQualityChanged)
    Q_PROPERTY(int runningSteps READ runningSteps WRITE setRunningSteps NOTIFY runningStepsChanged)

public:
    /**
     * Creates a new Hvac object.
     *
     * @param parent The parent object.
     */
    explicit SmartDevice(QObject *parent = 0);

    int heartRate() const;
    Q_INVOKABLE void setHeartRate(int heartRate);
    int pulse() const;
    Q_INVOKABLE void setPulset(int pulse);
    int sleepQuality() const;
    Q_INVOKABLE void setSleepQuality(int sleepQuality);
    int runningSteps() const;
    Q_INVOKABLE void setRunningSteps(int runningSteps);
    Q_INVOKABLE void tts_read_string(QString read_out_info);
    Q_INVOKABLE void tts_read_string_arg(int arg1, int arg2, int arg3);
    Q_INVOKABLE void fault_information_simulator(bool state);


Q_SIGNALS:
    void heartRateChanged(int);
    void pulseChanged(int);
    void sleepQualityChanged(int);
    void runningStepsChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
