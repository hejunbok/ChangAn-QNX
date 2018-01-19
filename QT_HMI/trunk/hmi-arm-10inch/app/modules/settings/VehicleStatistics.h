#ifndef VEHICLESTATISTICS_H
#define VEHICLESTATISTICS_H

#include <QDateTime>
#include <QObject>

/**
 * @short A class that encapsulates the vehicle statistics
 *
 * @note These values should actually be read from a PPS object,
 *       however there is non defined yet in the specification.
 */
class VehicleStatistics : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDateTime departureTime READ departureTime RESET resetDepartureTime NOTIFY departureTimeChanged)
    //The following two properties are workarounds for QTBUG-35693 (date/time formatting broken in QtQuick)
    //and can be removed again once the Qt bug is fixed.
    Q_PROPERTY(QString departureDateAsString READ departureDateAsString NOTIFY departureTimeChanged)
    Q_PROPERTY(QString departureTimeAsString READ departureTimeAsString NOTIFY departureTimeChanged)
    Q_PROPERTY(int tripDuration READ tripDuration RESET resetTripDuration NOTIFY tripDurationChanged)
    Q_PROPERTY(float mileage READ mileage RESET resetMileage NOTIFY mileageChanged)
    Q_PROPERTY(int distance READ distance RESET resetDistance NOTIFY distanceChanged)
    Q_PROPERTY(float averageSpeed READ averageSpeed RESET resetAverageSpeed NOTIFY averageSpeedChanged)

public:
    explicit VehicleStatistics(QObject *parent = 0);

public Q_SLOTS:
    void resetProperty(const QString &name);
    void onToggleFLDoor(void);
    void onToggleFRDoor(void);
    void onToggleRLDoor(void);
    void onToggleRRDoor(void);
    void onToggleBackDoor(void);
    void onToggleWindows(void);
    void onToggleFrontLight(void);
    void onToggleFogLight(void);

Q_SIGNALS:
    void departureTimeChanged();
    void tripDurationChanged();
    void mileageChanged();
    void distanceChanged();
    void averageSpeedChanged();

private:
    QString departureDateAsString() const;
    QString departureTimeAsString() const;
    QDateTime departureTime() const;
    void resetDepartureTime();
    int tripDuration() const;
    void resetTripDuration();
    float mileage() const;
    void resetMileage();
    int distance() const;
    void resetDistance();
    float averageSpeed() const;
    void resetAverageSpeed();

    QDateTime m_departureTime;
    int m_tripDuration;
    float m_mileage;
    int m_distance;
    float m_averageSpeed;
};

#endif
