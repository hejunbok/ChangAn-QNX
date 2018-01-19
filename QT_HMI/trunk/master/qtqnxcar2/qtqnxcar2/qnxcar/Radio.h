#ifndef QTQNXCAR2_RADIO_H
#define QTQNXCAR2_RADIO_H

#include "qtqnxcar2_export.h"

#include <QObject>
#include <QVariant>

namespace QnxCar {

/**
 * PPS wrapper for radio related objects
 *
 * Covers the following PPS objects:
 * - @c /pps/radio/control
 * - @c /pps/radio/status
 * - @c /pps/radio/tuners
 */
class QTQNXCAR2_EXPORT Radio : public QObject
{
    Q_OBJECT

    /// The currently used tuner (AM or FM)
    Q_PROPERTY(Tuner tuner READ tuner WRITE setTuner NOTIFY tunerChanged)

    /// The list of 6 preset stations (float value) for the currently used tuner
    Q_PROPERTY(QVariantList presetStations READ presetStations NOTIFY presetStationsChanged)

    /// The current station
    Q_PROPERTY(double station READ station NOTIFY stationChanged)

    /// The currently played song
    Q_PROPERTY(QString song READ song NOTIFY songChanged)

    /// The artist of the currently played song
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)

    /// The genre of the current station
    Q_PROPERTY(QString genre READ genre NOTIFY genreChanged)

    /// The name of the current station
    Q_PROPERTY(QString stationName READ stationName NOTIFY stationNameChanged)

    /// Whether high definition mode is enabled
    Q_PROPERTY(bool highDefinitionEnabled READ highDefinitionEnabled NOTIFY highDefinitionEnabledChanged)

    /// Whether the radio is currently seeking in the up direction
    Q_PROPERTY(bool seekingUp READ seekingUp NOTIFY seekingUpChanged)

    /// Whether the radio is currently seeking in the down direction
    Q_PROPERTY(bool seekingDown READ seekingDown NOTIFY seekingDownChanged)

    /// Whether the radio is currently scanning frequencies in any direction
    Q_PROPERTY(ScanDirection scanDirection READ scanDirection NOTIFY scanDirectionChanged)

    /// The lowest frequency on the current tuner
    Q_PROPERTY(double frequencyMinimum READ frequencyMinimum NOTIFY tunerChanged)

    /// The highest frequency on the current tuner
    Q_PROPERTY(double frequencyMaximum READ frequencyMaximum NOTIFY tunerChanged)

    /// The frequency step size to switch between stations on the current tuner
    Q_PROPERTY(double frequencyStep READ frequencyStep NOTIFY tunerChanged)

    Q_ENUMS(Tuner)
    Q_ENUMS(ScanDirection)

public:
    /// Describes the available tuners
    enum Tuner
    {
        AmTuner, ///< The AM tuner
        FmTuner  ///< The FM tuner
    };

    /// Describes the possible directions for scanning
    enum ScanDirection
    {
        ScanNone, ///< Do not scan frequencies
        ScanUp,   ///< Scan frequencies upwards
        ScanDown  ///< Scan frequencies downwards
    };

    /**
     * Creates a new Radio object.
     *
     * @param parent The parent object.
     */
    explicit Radio(QObject *parent = 0);

    Tuner tuner() const;
    void setTuner(Tuner tuner);

    QVariantList presetStations() const;
    QVariantList presetStations(Tuner tuner) const;
    void setPresetStations(Tuner tuner, const QVariantList& stations);

    double station() const;
    QString song() const;
    QString artist() const;
    QString genre() const;
    QString stationName() const;
    bool highDefinitionEnabled() const;
    bool seekingUp() const;
    bool seekingDown() const;
    ScanDirection scanDirection() const;
    double frequencyMinimum() const;
    double frequencyMaximum() const;
    double frequencyStep() const;

    /**
     * Return the default scan interval between seeks when in scan mode
     *
     * @return Scan interval in ms
     * @sa setScanInterval();
     */
    int scanInterval() const;

    /**
     * Sets the default scan interval
     *
     * @param ms interval in milliseconds
     * @sa scanInterval()
     */
    void setScanInterval(int ms);

    /**
     * Returns whether the simulation mode is enabled
     *
     * @return @c true if simulation mode is enabled, @c false for actual tuner access
     * @sa setSimulationModeEnabled()
     */
    bool simulationModeEnabled() const;

    /**
     * Sets the simulation state
     *
     * @param enabled if @c true simulate seeks, if @c false perform seek through PPS
     * @sa simulationModeEnabled()
     */
    void setSimulationModeEnabled(bool enabled);

public Q_SLOTS:
    /// Seek towards higher frequencies
    void seekUp();
    /// Seek towards lower frequencies
    void seekDown();
    /// Stop seeking
    void stopSeek();

    /// Like seekUp() but regularily reinitiated until stopped
    void scanUp();
    /// Like seekDown() but regularily reinitiated until stopped
    void scanDown();
    /// Stop scanning
    void stopScan();

    /// Change frequency to a given @p station
    void tune(double station);

    /**
     * Updates a preset to the currently set station
     *
     * @param index preset station index in the preset list to update
     */
    void updatePreset(int index);

Q_SIGNALS:
    void tunerChanged(QnxCar::Radio::Tuner tuner);
    void presetStationsChanged(const QVariantList &presetStations);
    void stationChanged(double station);
    void songChanged(const QString &song);
    void artistChanged(const QString &artist);
    void genreChanged(const QString &genre);
    void stationNameChanged(const QString &stationName);
    void highDefinitionEnabledChanged(bool enabled);
    void seekingUpChanged(bool seeking);
    void seekingDownChanged(bool seeking);
    void scanDirectionChanged(QnxCar::Radio::ScanDirection direction);

private:
    class Private;
    Private* const d;
};

}

#endif
