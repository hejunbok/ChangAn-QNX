#ifndef QTQNXCAR2_AUDIOCONTROL_H
#define QTQNXCAR2_AUDIOCONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to audio settings
 *
 * Allows controlling and monitoring #volume, #balance, #bass, #fade, #mid and #treble.
 *
 * Wrapper around PPS objects:
 *
 * - mixer: @c /pps/services/audio/mixer
 * - status: @c /pps/services/audio/status
 * - control: @c /pps/services/audio/control
 *
 */
class QTQNXCAR2_EXPORT AudioControl : public QObject
{
    Q_OBJECT

    /**
     * Volume of the speakers, range 0 - 100
     *
     * @accessors speakerVolume(), setSpeakerVolume()
     */
    Q_PROPERTY(double speakerVolume READ speakerVolume WRITE setSpeakerVolume NOTIFY speakerVolumeChanged)

    /**
     * Volume of the handset, range 0 - 100
     *
     * @accessors handsetVolume(), setHandsetVolume()
     */
    Q_PROPERTY(double handsetVolume READ handsetVolume WRITE setHandsetVolume NOTIFY handsetVolumeChanged)

    /**
     * The currently active output device
     *
     * @accessors currentOutputDevice()
     */
    Q_PROPERTY(DeviceType currentOutputDevice READ currentOutputDevice NOTIFY currentOutputDeviceChanged)

    /**
     * The volume of the currently active output device ("handset" or "speaker")
     *
     * @accessors currentOutputVolume(), setCurrentOutputVolume
     */
    Q_PROPERTY(double currentOutputVolume READ currentOutputVolume WRITE setCurrentOutputVolume NOTIFY currentOutputVolumeChanged)

    /**
     * Balance, range 0 - 100
     *
     * @accessors balance(), setBalance()
     */
    Q_PROPERTY(double balance READ balance WRITE setBalance NOTIFY balanceChanged)

    /**
     * Bass, range 0 - 100
     *
     * @accessors bass(), setBass()
     */
    Q_PROPERTY(double bass READ bass WRITE setBass NOTIFY bassChanged)

    /**
     * Fade, range 0 - 100
     *
     * @accessors fade(), setFade()
     */
    Q_PROPERTY(double fade READ fade WRITE setFade NOTIFY fadeChanged)

    /**
     * Mid, range 0 - 100
     *
     * @accessors mid(), setMid()
     */
    Q_PROPERTY(double mid READ mid WRITE setMid NOTIFY midChanged)

    /**
     * Treble, range 0 - 100
     *
     * @accessors treble(), setTreble()
     */
    Q_PROPERTY(double treble READ treble WRITE setTreble NOTIFY trebleChanged)

    Q_ENUMS(DeviceType)

public:
    /**
     * Devices for audio output
     */
    enum DeviceType {
        Speaker, ///< Speakers
        Handset ///< Phone handset
    };

    /**
     * Conntects to PPS and retrieves initial values
     *
     * @param parent the QObject parent
     */
    explicit AudioControl(QObject *parent = 0);

    /// #speakerVolume
    double speakerVolume() const;
    /// #speakerVolume
    void setSpeakerVolume(double volume);
    /// #handsetVolume
    double handsetVolume() const;
    /// #handsetVolume
    void setHandsetVolume(double volume);
    /// #currentOutputVolume
    double currentOutputVolume() const;
    /// #currentOutputVolume
    void setCurrentOutputVolume(double volume);
    /// #balance
    double balance() const;
    /// #balance
    void setBalance(double balance);
    /// #bass
    double bass() const;
    /// #bass
    void setBass(double bass);
    /// #fade
    double fade() const;
    /// #fade
    void setFade(double fade);
    /// #mid
    double mid() const;
    /// #mid
    void setMid(double mid);
    /// #treble
    double treble() const;
    /// #treble
    void setTreble(double treble);
    /// #currentOutputDevice
    DeviceType currentOutputDevice() const;

Q_SIGNALS:
    void speakerVolumeChanged(double volume);
    void handsetVolumeChanged(double volume);
    void balanceChanged(double balance);
    void bassChanged(double bass);
    void fadeChanged(double fade);
    void midChanged(double mid);
    void trebleChanged(double treble);
    void currentOutputDeviceChanged(DeviceType type);
    void currentOutputVolumeChanged(double);

private:
    class Private;
    Private* const d;
};

}

#endif
