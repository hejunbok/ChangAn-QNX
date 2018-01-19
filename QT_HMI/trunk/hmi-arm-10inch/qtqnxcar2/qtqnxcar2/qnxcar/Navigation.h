#ifndef QTQNXCAR2_NAVIGATION_H
#define QTQNXCAR2_NAVIGATION_H

#include "qtqnxcar2_export.h"

#include <QObject>

#include "Location.h"

namespace QnxCar {
    class ControlCommand;
    class ManeuversModel;

/**
 * Wrapper class for the @c /pps/qnxcar/navigation/status and @c /pps/qnxcar/navigation/control PPS objects
 */
class QTQNXCAR2_EXPORT Navigation : public QObject
{
    Q_OBJECT

    /**
     * Whether the system is currently navigating
     *
     * @accessors navigating()
     */
    Q_PROPERTY(bool navigating READ navigating NOTIFY navigatingChanged)

    /**
     * Remaining distance to destination in meters
     *
     * @accessors totalDistanceRemaining()
     */
    Q_PROPERTY(int totalDistanceRemaining READ totalDistanceRemaining NOTIFY totalDistanceRemainingChanged)

    /**
     * Estimate time to destination in minutes
     *
     * @accessors totalTimeRemaining()
     */
    Q_PROPERTY(int totalTimeRemaining READ totalTimeRemaining NOTIFY totalTimeRemainingChanged)

    /**
     * Location data of the destination
     *
     * @accessors destination()
     */
    Q_PROPERTY(QnxCar::Location destination READ destination NOTIFY destinationChanged)

    /**
     * Model containing the navigation instructions to #destination
     *
     * @accessors maneuversModel()
     */
    Q_PROPERTY(QnxCar::ManeuversModel* maneuversModel READ maneuversModel CONSTANT)

    Q_ENUMS(Command)
public:

    /**
     * Creates a navigation object
     *
     * @param parent parent QObject
     */
    explicit Navigation(QObject *parent = 0);

    /**
     * Indicates whether the system is currently navigating
     *
     * @sa #navigating
     */
    bool navigating() const;

    /**
     * Distance to destination (in meters)
     *
     * @sa #totalDistanceRemaining
     */
    int totalDistanceRemaining() const;

    /**
     * Estimated time of arrival (in minutes)
     *
     * @sa #totalTimeRemaining
     */
    int totalTimeRemaining() const;

    /**
     * Data for current destination
     *
     * @sa #destination
     */
    Location destination() const;

    /**
     * Returns the model of maneuvers
     *
     * @sa #maneuversModel
     */
    ManeuversModel *maneuversModel() const;

    /**
     * Starts a navigation to location @p location
     *
     * @param location must be a valid location object
     */
    QnxCar::ControlCommand *navigateTo(const Location &location);

    /**
     * Stops the current navigation
     */
    Q_INVOKABLE QnxCar::ControlCommand *cancelNavigation();

    /**
     * Translates the view on the map
     *
     * @param deltaX number of pixels to move the map horizontally
     * @param deltaY number of pixels to move the map vertically
     */
    Q_INVOKABLE QnxCar::ControlCommand *panMap(int deltaX, int deltaY);

    /**
     * Zooms the map view
     * @param scale the map scale in meters/pixel
     */
    Q_INVOKABLE QnxCar::ControlCommand *zoomMap(qreal scale);

    /**
     * Get points of interest for category with ID @p categoryId
     *
     * Once completed, pointsOfInterestedResultsReceived() will be emitted.
     * The POI results are written to the navigation database and can be
     * accessed using a LocationModel.
     */
    Q_INVOKABLE QnxCar::ControlCommand *getPointsOfInterest(int categoryId);

    /**
     * Get points of interest with string containing @p name
     *
     * Once completed, pointsOfInterestedResultsReceived() will be emitted.
     * The POI results are written to the navigation database and can be
     * accessed using a LocationModel.
     */
    Q_INVOKABLE QnxCar::ControlCommand *searchPointsOfInterest(const QString &name);

    /**
     * Starts a search with the given arguments
     *
     * Includes only non-empty search criteria, e.g. if @ country is empty it will not include it when searching.
     * Updates the location table of the navigation database with its results.
     *
     * @param country the country name to search for
     * @param city the city name to search for
     * @param street the street name to search for
     * @param number the street number to search for
     * @return a command object that signals end of search
     * @sa LocationModel::update()
     */
    Q_INVOKABLE QnxCar::ControlCommand *search(const QString &country, const QString &city, const QString &street, const QString &number);

Q_SIGNALS:
    void navigatingChanged(bool navigating);
    void totalDistanceRemainingChanged(int totalDistanceRemaining);
    void totalTimeRemainingChanged(int totalTimeRemaining);
    void destinationChanged(const QnxCar::Location &destination);
    void pointsOfInterestResultsReceived();
    void searchResultsReceived();

private:
    class Private;
    Private *const d;
};

}

#endif
