#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Wifi.h"

#include <qpps/object.h>
#include "qpps/dirwatcher.h"

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {
    /**
     * Class controls and interfaces with Tetherman
     * @param parent QObject
     */
    class QTQNXCAR2_EXPORT HotSpot : public QObject{
        Q_OBJECT

        /**
         * Property indicates that service is available or not
         *
         * @accessor isAvailable(), setAvailable;
         */
        Q_PROPERTY(bool available READ isAvailable WRITE setAvailable NOTIFY availableChanged)

        /**
         * Current profile name
         *
         * @accessors name()
         */
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        /**
         * Current profile password
         *
         * @accessors password()
         */
        Q_PROPERTY(QString password READ getPassphrase WRITE setPassphrase NOTIFY passwordChanged)
        /**
         * Current profile band
         *
         * @accessors band()
         */
        Q_PROPERTY(QString band READ getBand WRITE setBand NOTIFY bandChanged)
        /**
         * Indicated if profile enabled
         *
         * @accessors enabled()
         */
        Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    private:
        QPps::Object *m_tm_ppsControlObject;                // tetherman control object (server mode)
        QPps::Object *m_tm_ppsStatusObject;                 // tetherman status
        QPps::DirWatcher *m_ppsServicesDirWatcher;          // dirwatcher to monitor if control object available

        bool m_available;                                   // if service available or not

        QString m_curentName;                               // current profile name
        bool m_enabled;                                     // if profile enabled (active)
        QString m_band;                                     // current band
        QString m_passphrase;                               // current password
        QString m_securityType;                             // current security type (hardcoded)
        QString m_curentProfile;                            // current profile in string form
        int m_messageId;                                    // current message id

        /**
          Event handler to process pps reply messages coming from Tetherman control object
          @param message Message reply message
        */
        void onTMControlReply(Message * message);
        /**
        Function retrieves profile from tetherman, if nothing exists that means HOTSPOT was never configured
        */
        void retrieveProfile();
        /**
          Function to update current profile, send update profile command to tetherman
          @param profile QString current profile in string form
        */
        void updateProfile();

        /**
         * Function takes string from PPS object whcih represents current profile and parses it in order to fetch all necessary
         * HotSpot profile fields
         * @brief parseCurrentProfile
         * @param profile QString string in JSON format loaded from PPS
         */
        void parseCurrentProfile(QString profile);

        /**
         * Method starts wifi thethering, by providing all necessary profile information.
         * @brief startTethering
         */
        void startTethering();
        /**
         * Methods stops wifi tethering
         * @brief stopTethering
         */
        void stopTethering();

        /**
         * Function initialises PPS if "control" object available
         * @brief initialises PPS
         * @param object PPS object name
         */
        void initPPS(const QString &object);

    public:
        /**
          Default constructor
        */
        explicit HotSpot(QObject *parent = 0);
        /** To get current orifile name
         *  @return QString current profile name
        */

        /**
         * @brief available return boolean indicating that service is available or not
         * @return true if HOTSPOT service available
         */
        bool isAvailable() const;

        /**
         * @brief sets available for the service and emits availableChanged() signal if value changed
         */
        void setAvailable(bool value);

        QString getName();
        /**
         *  To set current profile name, called mostly from QML
         *  @param value, reference to the new name
         */
        void setName(const QString &value);
        /**
         * Getter to obtain default hotspot band
         * @brief getBand
         * @return QString string representaion of the band name
         */
        QString getBand();
        /**
         * Setter to set new band value
         * @brief setBand
         * @param value QString new band value
         */
        void setBand(const QString &value);
        /**
         * Getter to obtain current passphrase
         * @brief getPassphrase
         * @return QString current passphrase
         */
        QString getPassphrase();
        /**
         * Setter to set new passphrase
         * @brief setPassphrase
         * @param value QString new passphrase value
         */
        void setPassphrase(const QString &value);
        /**
         * Getter to obtain current security type
         * @brief getSecurityType
         * @return QString current securirty type
         */
        QString getSecurityType();
        /**
         * Setter to set new security type
         * @brief setSecurityType
         * @param value QString new security type value
         */
        void setSecurityType(const QString &value);
        /**
         * Getter to indicate if profile is enabled
         * @brief isEnabled
         * @return bool true if prifile enabled
         */
        bool isEnabled();
        /**
         * Set profile enable or disabled
         * @brief setEnabled
         * @param value bool true - to enable, false to disable
         */
        void setEnabled(bool value);
        /**
         * True - does necessary action to have hotspon operational, False - deactivates active hotspot
         * @brief enableHotspot
         * @param value true - to enable and turn on current hotspot, false - to disable and turn off
         */
        void enableHotspot(bool value);

    public Q_SLOTS:
        /**
         * Slot hoked up to monitor tetherman status PPS object
         * @param name attribute name
         * @param attribute attribute value
         */
        void ppsTetherManStatusChanged(const QString &name, const QPps::Variant &attribute);
        /**
         * Slot hoked up to monitor PSS replies for tetherman control object
         * @param changes list attrubutes whih were changed
         */
        void ppsTetherManReply(const QPps::Changeset &changes);
        /**
         *  Slot hooked up to Save button in HotSpot settings
         *  Will start HotSpot when no profile set, and update Hotspot profile settings when it is set and active
         *  avaliable for access from QML
         */
        void onSaveProfile();

        /**
         * Slot hooked to to switch and will run specific code to either start tethering or display Profile settings
         * @brief onEnableHotspot
         */
        void onEnableHotspot(bool value);

    Q_SIGNALS:
        /**
         * Signals that profile name changed
         */
        void nameChanged();
        /**
         * Signals that is enable or disable (enabled changed)
         */
        void enabledChanged();
        /**
         * Signals that band changed
         */
        void bandChanged();
        /**
         * Signal that password changed
         */
        void passwordChanged();

        /**
         * Signal that profile updating
         */
        void profileUpdating();

        /**
         * Signal that profile update sucessfully
         */
        void profileUpdated();

        /**
         * Signals that first time configuration required
         */
        void firstTimeConfig();
        /**
         * Signals that user starting hotspot
         */
        void hotspotStarting();
        /**
         * Signals that user stopping hotspot
         */
        void hotspotStopping();
        /**
         * Signals that service availability changed
         */
        void availableChanged();
    };
}
#endif // HOTSPOT_H
