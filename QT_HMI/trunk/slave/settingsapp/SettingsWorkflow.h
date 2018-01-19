#ifndef SETTINGSWORKFLOW_H
#define SETTINGSWORKFLOW_H

#include <QObject>

class SettingsWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SettingsWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_ENUMS(Page)

public:
    enum Page {
        BluetoothConnectivityPage,
        SoftwareUpdatesPage,
        WiredNetworkPage,
        WifiClientPage,
        HotSpotPage
    };

    explicit SettingsWorkflow(QMap<int,bool> availPages,QObject *parent = 0);

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

#endif // SETTINGSWORKFLOW_H
