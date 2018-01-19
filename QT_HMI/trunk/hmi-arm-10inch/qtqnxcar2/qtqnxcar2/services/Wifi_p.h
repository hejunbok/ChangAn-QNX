#ifndef WIFI_P_H
#define WIFI_P_H

#include <qpps/object.h>
#include "qpps/dirwatcher.h"
#include "pps/ControlCommand.h"

#include "Wifi.h"
#include "WifiAPModel.h"

#include <QTimer>

class QnxCar::Wifi::Private : public QObject
{
    Q_OBJECT

    private:

    QPps::Object *m_wf_ppsControlObject;        // wifi control PPS obect
    QPps::Object *m_wf_ppsStatusObject;         // wifi status PPS object
    QPps::DirWatcher *m_ppsServicesDirWatcher;  // dirwatcher to monitor if control object available

    int m_messageId;                            // current message id

    ControlCommand *requestScanResults();

    public:
    explicit Private(Wifi *q = 0);
    virtual ~Private();

    bool m_available;
    bool m_connected;
    bool m_powered;
    QString m_apName;

    /* Scan timer */
    QTimer *scanTimer;

    WifiAPModel* accessPoints;

    /* initiates PPS objects*/
    void initPPS(const QString &object);
    /* Intiates wifi  and power it up*/
    void doStart();
    /* Powers WIFI system down*/
    void doStop();
    /* initiates scan */
    ControlCommand *doScan();
    /* initiates scan only when WIFI is powered*/
    void doStartScan();
    /* stops scan */
    void doStopScan();
    /* initiates AP add, requires as first  step before we can connect to selected network */
    ControlCommand *doAdd(QString bssid, QString identity, QString password);
    /* Initiates actual connection to the selected AP */
    ControlCommand *doEnable(QString nid);
    /* Initiates actual connection to the selected AP by selecting the network*/
    ControlCommand *doSelect(QString nid);
    /* Initiates disconnect from the selected AP by selecting the network*/
    ControlCommand *doDisable();
    /* funtion processed scan result, does a lot of PPS parsing */
    void processScanResult(const QPps::Variant &attribute);
    /* function handles changes in status object */
    void processStatusResult(const QPps::Variant &attribute);

    /* brief sets available for the service and emits availableChanged() signal if value changed */
    void setAvailable(bool value);

    Wifi *const q;

    private Q_SLOTS:
    /*
     * Slot hoked up to monitor wifi status PPS object
     * @param name attribute name
     * @param attribute attribute value
     */
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);
    /* handles timer events */
    void onTimer();
    /* handles reply message from control object */
    void onReply(QnxCar::ControlCommand*);
    /* handles reply message from control object when disabling*/
    void onDisableReply(QnxCar::ControlCommand*);
    /* handles scan results */
    void onScanResultReady(QnxCar::ControlCommand*);
    /* handles reply messages from ADD operation */
    void onAddReply(QnxCar::ControlCommand*);
    /* handles actually enabling and connecting of enable AP to the board */
    void onEnableReply(QnxCar::ControlCommand*);
    /* handles actually selecting and connecting of enable AP to the board */
    void onSelectReply(QnxCar::ControlCommand*);
};

#endif // WIFI_P_H
