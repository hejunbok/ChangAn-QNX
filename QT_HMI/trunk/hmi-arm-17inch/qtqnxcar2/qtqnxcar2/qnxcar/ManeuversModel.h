#ifndef QTQNXCAR2_MANEUVERSMODEL_H
#define QTQNXCAR2_MANEUVERSMODEL_H

#include "qtqnxcar2_export.h"

#include <QAbstractListModel>
#include <QString>

namespace QnxCar {

struct Maneuver;

/**
 * Model listing the upcoming maneuvers
 *
 * A maneuver is an instruction to the driver, such as "Turn left into XY road in 300 meters",
 * "Go straight for 5 km", etc.
 *
 * This model contains all upcoming maneuvers provided by the navigation backend, where
 * the next maneuver is in the first row (row 0).
 */
class QTQNXCAR2_EXPORT ManeuversModel : public QAbstractListModel
{
    Q_OBJECT

    /**
     * Holds the number of Maneuver items in the model
     *
     * @accessors rowCount()
     */
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    Q_ENUMS(Command)

public:
    /// Item roles
    enum Role {
        /// The street of the maneuver, Role name @c "street"
        StreetRole = Qt::UserRole,
        /// The instruction to the driver. Returns a #Command value. Role name @c "command"
        CommandRole,
        /// The distance until the maneuver should be performed by the driver (integer, in meters). Role name @c "distance"
        DistanceRole
    };

    /**
     * @brief The Command enum
     */
    enum Command {
        /// Arrived at destination
        ArrivedAtDestination,
        /// Destination is on the left hand side
        DestinationIsOnTheLeft,
        /// Destination is on the right hand side
        DestinationIsOnTheRight,
        /// Follow the roundabout on the left
        FollowRoundaboutOnTheLeft,
        /// Perform a U-turn on the left
        UTurnOnTheLeft,
        /// Keep going (straight)
        NoChange,
        /// Follow the roundabout on the right
        FollowRoundaboutOnTheRight,
        /// Turn left
        TurnLeft,
        /// Turn right
        TurnRight,
        /// Perform a U-turn on the right
        UTurnOnTheRight
    };

    /**
     * Maps a command string to a #Command value
     *
     * @param str the command string to parse
     * @param ok @c true if @p str was a valid command string, @c false on a parsing error
     * @return #NoChange on parsing error, otherwise the #Command value of @p str
     * @sa commandToString()
     */
    static Command commandFromString(const QString &str, bool *ok = 0);

    /**
     * Maps a #Command value to a string representation
     *
     * @param command the command to format as a string
     * @return the string representation of @p command
     * @sa commandFromString()
     */
    static QString commandToString(Command command);

    /**
     * Creates an empty model
     *
     * Use setManeuvers() to fill it with content.
     *
     * @param parent the QObject parent
     */
    explicit ManeuversModel(QObject *parent = 0);

    /// Destroys the instance
    ~ManeuversModel();

    /// @sa #rowCount
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /**
     * Returns the Manuever items contained in the model@brief maneuvers
     *
     * @return the model content
     * @sa setManeuvers()
     */
    QVector<Maneuver> maneuvers() const;

    /**
     * Sets the model content
     *
     * @param maneuvers the Maneuvers to export as model content
     * @sa maneuvers()
     */
    void setManeuvers(const QVector<Maneuver> &maneuvers);

Q_SIGNALS:
    void rowCountChanged(int rowCount);

private:
    class Private;
    Private *const d;
};

}

#endif
