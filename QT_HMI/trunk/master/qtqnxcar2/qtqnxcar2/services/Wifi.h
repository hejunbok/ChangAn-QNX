#ifndef WIFI_H
#define WIFI_H

#include <qpps/object.h>
#include <QObject>

#include "qtqnxcar2_export.h"
#include "WifiAPModel.h"

namespace QnxCar {

    /**
     * @brief The Message class
     * Class container for PPS messages used in server object communication
     */
    class QTQNXCAR2_EXPORT Message : public QObject{

        Q_OBJECT
        /** Message of the message */
        Q_PROPERTY(QString msg READ getMessage WRITE setMessage)
        /** Id of the mesage */
        Q_PROPERTY(QString id READ getId WRITE setId)
        /** data or payload of the message */
        Q_PROPERTY(QString dat READ getData WRITE setData)
        /** response ot the message */
        Q_PROPERTY(QString res READ getResponse WRITE setResponse)
        /** error code of the message or 0 if all good */
        Q_PROPERTY(QString err READ getErrorCode WRITE setErrorCode)
        /** errore string available only when error code is not 0 */
        Q_PROPERTY(QString errstr READ getErrorString WRITE setErrorString)

    private:
        QString m_msg;          // msg message
        QString m_id;           // id of the message
        QString m_dat;          // data or payload
        QString m_res;          // response
        QString m_err;          // error code in response, 0 if all good
        QString m_errstr;       // error string if err is not 0

    public:

        explicit Message(QObject *parent = 0);

        /**
         * @brief Getter to obtain current message string
         * @return QString current message
         */
        QString getMessage();
        /**
         * @brief Getter to obtain message id
         * @return QString message id
         */
        QString getId();
        /**
         * @brief Getter to obtain data or payload of this message
         * @return QString message data
         */
        QString getData();
        /**
         * @brief Getter to obtain response
         * @return QString message response
         */
        QString getResponse();
        /**
         * @brief Getter to obtain error code
         * @return QString message error code
         */
        QString getErrorCode();
        /**
        * @brief Getter to obtain error string
         * @return QString message error string
         */
        QString getErrorString();

        /**
         * To set message string
         * @brief setMessage
         * @param value QString message string
         */
        void setMessage(QString value);
        /**
         * To set message id
         * @brief setId
         * @param value QString message id
         */
        void setId(QString value);
        /**
         * To set message data
         * @brief setData
         * @param value QString message data
         */
        void setData(QString value);
        /**
         * To set message response
         * @brief setResponse
         * @param value QString message response
         */
        void setResponse(QString value);
        /**
         * To set message error code
         * @brief setErrorCode
         * @param value QString message error code
         */
        void setErrorCode(QString value);
        /**
         * To set message error string
         * @brief setErrorString
         * @param value QString message error string
         */
        void setErrorString(QString value);
    };

    /**
     * Class controls and interfaces with Wifi
     * @param parent QObject
     */
    class QTQNXCAR2_EXPORT Wifi : public QObject{
    Q_OBJECT
    /**
     * Property indicates that service is available or not
     *
     * @accessors isAvailable() , setAvailable()
     */
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
    /**
     * Indicator of connection of Wifi service
     *
     * @accessors connected()
     */
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    /**
     * Status of the power for WIFI device
     *
     * @accessors powered()
     */
    Q_PROPERTY(bool powered READ powered NOTIFY poweredChanged)
    /**
     * Current active access point name
     *
     * @accessors apName()
     */
    Q_PROPERTY(QString apName READ apName NOTIFY apNameChanged)
    /**
     * Model containin all currently scanned access points
     *
     * @accessors apName()
     */
    Q_PROPERTY(QAbstractItemModel* accessPoints READ accessPoints CONSTANT)

    private:
        class Private;
        Private *const d;

    public:
        /**
         * Default constructor
         * @brief Wifi
         * @param parent reference to this class parent
         */
        explicit Wifi(QObject *parent = 0);

        /**
         * @brief available return boolean indcating that service is available or not
         * @return true if WIFI service available
         */
        bool isAvailable() const;

        /**
         * @brief connected returns boolen defining if Wifi client connected or not
         * @return boolean, true - if connected, false if not.
         */
        bool connected();
        /**
         * @brief powered returns boolen whihc defines if Wifi chip powered or not
         * @return boolean, true - if powered, false - not
         */
        bool powered();
        /**
         * @brief apName returns string name of the cusstent access point
         * @return QString, representing access point name
         */
        QString apName();

        /**
         * @brief accessPoints returns reference to access point model which contains all currently scannen access points
         * @return WifiAPModel model contains access points
         */
        WifiAPModel* accessPoints() const;

    public Q_SLOTS:
        /**
         * Slot to invoke when we need to power on WIFI (start WIFI)
         * @brief startWifi
         */
        void startWifi();
        /**
         * Slot to invoke when we need to power down WIFI (stop WIFI)
         * @brief stopWifi
         */
        void stopWifi(); // TODO Implement

        /**
         * @brief starts continuous scan,
         * Starts continuous scan, WIFI shoul be powered on.
         */
        void startScan();

        /**
         * @brief stops continuous scan,
         * Stops continuous scan, WIFI shoul be powered on.
         */
        void stopScan();

        /**
         * Function initiates connection to provided bssid, with user credentials
         * @brief connect, connets to selected access point
         * @param bssid - unique ID of the access point
         * @param identity - access points password
         * @param password - accedd point username
         */
        void connect(QString bssid, QString identity, QString password);

        /**
         * @brief disconnects the currently conencted access point
         */
        void disconnect();

        /**
         * @brief status returns current status of the status PPS object
         */
        void status();

        /**
         * Function cancel curret operations, for example connect to AP
         * @brief cancel - cancels currnt operations ()
         */
        void cancel();

    Q_SIGNALS:
        /**
         * To signal about WIFI status change
         * @brief wifiStarted
         */
        void statusChanged();
        /**
         * To signal that connected chnaged
         * @brief connectedChanged
         */
        void connectedChanged();
        /**
         * To signal about that power property changed
         * @brief poweredChanged
         */
        void poweredChanged();
        /**
         * To notify that name of the active access point changed
         * @brief apNameChanged
         */
        void apNameChanged();
        /**
         * Indicates that scan had finish
         * @brief scanSuccess
         */
        void scanSuccess();
        /**
         * Indicated tahat scan failed
         * @brief scanFail
         */
        void scanFail();
        /**
         * Notifies when scan finished and it has results
         * @brief scanResultSuccess
         */
        void scanResultSuccess();
        /**
         * Notifies when user connecting to the network
         * @brief connectingToNetwork
         */
        void connectingToNetwork();
        /**
         * Indicates that connection to AP is successful
         * @brief successfulConnect
         */
        void successfulConnect();
        /**
         * Indicates that serice failed to connect to AP, error message provided
         * @brief failedConnect
         */
        void failedConnect();
        /**
         * Reports connection progress
         * @brief connectionProgress
         */
        void connectionProgress();
        /**
         * Notifies when service availability changes
         * @brief availableChanged
         */
        void availableChanged();

    };
}
#endif // WIFI_H
