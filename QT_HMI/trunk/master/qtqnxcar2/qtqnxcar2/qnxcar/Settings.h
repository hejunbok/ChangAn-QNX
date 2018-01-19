#ifndef QTQNXCAR2_SETTINGS_H
#define QTQNXCAR2_SETTINGS_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Gives access to UI profile settings for the QNXCar HMI
 *
 * The QnxCar HMI allows two different profiles (High and Mid)
 * to configure the complexity of the UI.
 * Setting the "Mid" profile will result in a simpler UI using less animations.
 *
 * Wraps the /pps/qnxcar/system/settings PPS object
 */
class QTQNXCAR2_EXPORT Settings : public QObject
{
    Q_OBJECT

    /// The profile for the app launcher module
    Q_PROPERTY(Profile appSectionProfile READ appSectionProfile WRITE setAppSectionProfile NOTIFY appSectionProfileChanged)

    /// The profile for the settings module
    Q_PROPERTY(Profile carControlProfile READ carControlProfile WRITE setCarControlProfile NOTIFY carControlProfileChanged)

    /// The profile for the communication module
    Q_PROPERTY(Profile communicationProfile READ communicationProfile WRITE setCommunicationProfile NOTIFY communicationProfileChanged)

    /// The profile for the media player module
    Q_PROPERTY(Profile mediaPlayerProfile READ mediaPlayerProfile WRITE setMediaPlayerProfile NOTIFY mediaPlayerProfileChanged)

    Q_ENUMS(Profile)

public:
    /// Describes the profile type
    enum Profile
    {
        High, ///< High profile, uses advanced animations
        Mid   ///< Mid profile, uses simplified UI and no animations
    };

    /**
     * Creates a new Settings object.
     *
     * @param parent The parent object.
     */
    explicit Settings(QObject *parent = 0);

    /// #appSectionProfile
    Profile appSectionProfile() const;

    /// #appSectionProfile
    void setAppSectionProfile(Profile profile);

    /// #carControlProfile
    Profile carControlProfile() const;

    /// #carControlProfile
    void setCarControlProfile(Profile profile);

    /// #communicationProfile
    Profile communicationProfile() const;

    /// #communicationProfile
    void setCommunicationProfile(Profile profile);

    /// #mediaPlayerProfile
    Profile mediaPlayerProfile() const;

    /// #mediaPlayerProfile
    void setMediaPlayerProfile(Profile profile);

Q_SIGNALS:
    void appSectionProfileChanged(QnxCar::Settings::Profile profile);
    void carControlProfileChanged(QnxCar::Settings::Profile profile);
    void communicationProfileChanged(QnxCar::Settings::Profile profile);
    void mediaPlayerProfileChanged(QnxCar::Settings::Profile profile);

private:
    class Private;
    Private* const d;
};

}

#endif
