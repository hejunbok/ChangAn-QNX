#ifndef __BT_WIFI_SETTINGSWORKFLOW_H__
#define __BT_WIFI_SETTINGSWORKFLOW_H__

#include <QObject>

class BtWifi_SettingsWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(BtWifi_SettingsWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_ENUMS(Page)

public:
    enum Page {
        BluetoothConnectivityPage,
        SoftwareUpdatesPage,
        WiredNetworkPage,
        WifiClientPage,
        HotSpotPage
    };

    explicit BtWifi_SettingsWorkflow(QMap<int,bool> availPages,QObject *parent = 0);

    Page currentPage() const;
    Q_INVOKABLE void enterPage(Page page);

public slots:
    void enterBluetoothConnectivityPage();
    void enterSoftwareUpdatesPage();
    void enterWiredNetworkPage();
    void enterWifiClientPage();
    void enterHotSpotPage();

signals:
    void onEnterBluetoothConnectivityPage();
    void onEnterSoftwareUpdatesPage();
    void onEnterWiredNetworkPage();
    void onEnterWifiClientPage();
    void onEnterHotSpotPage();

    void currentPageChanged(Page page);

private:
    void setCurrentPage(Page page);

    void initStateMachine(QMap<int,bool> availPages);

    Page m_currentPage;
};

#endif // __BT_WIFI_SETTINGSWORKFLOW_H__
