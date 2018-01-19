#include "BtWifi_SettingsWorkflow.h"

#include <QStateMachine>

namespace {
static const char *CURRENT_PAGE_PROPERTY = "currentPage";
}

BtWifi_SettingsWorkflow::BtWifi_SettingsWorkflow(QMap<int,bool> availPages,QObject *parent)
    : QObject(parent)
{
    initStateMachine(availPages);
}

void BtWifi_SettingsWorkflow::enterBluetoothConnectivityPage()
{
    emit onEnterBluetoothConnectivityPage();
}

void BtWifi_SettingsWorkflow::enterSoftwareUpdatesPage()
{
    emit onEnterSoftwareUpdatesPage();
}

void BtWifi_SettingsWorkflow::enterWiredNetworkPage()
{
    emit onEnterWiredNetworkPage();
}

void BtWifi_SettingsWorkflow::enterWifiClientPage()
{
    emit onEnterWifiClientPage();
}

void BtWifi_SettingsWorkflow::enterHotSpotPage()
{
    emit onEnterHotSpotPage();
}

void BtWifi_SettingsWorkflow::initStateMachine(QMap<int,bool> availPages)
{
    QStateMachine *machine = new QStateMachine(this);
    machine->setObjectName(QStringLiteral("BtWifi_SettingsWorkflow"));
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *bluetoothConnectivityPageState = new QState(machine);
    bluetoothConnectivityPageState->setObjectName(QStringLiteral("BluetoothConnectivityPage"));

    QState *softwareUpdatesPageState = new QState(machine);
    softwareUpdatesPageState->setObjectName(QStringLiteral("SoftwareUpdatesPage"));

    QState *wiredNetworkPageState = new QState(machine);
    wiredNetworkPageState->setObjectName(QStringLiteral("WiredNetworkPage"));

    QState *wifiClientPageState = new QState(machine);
    wifiClientPageState->setObjectName(QStringLiteral("WifiClientPage"));

    QState *hotSpotPageState = new QState(machine);
    hotSpotPageState->setObjectName(QStringLiteral("HotSpotPage"));

    // properties
    bluetoothConnectivityPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, BluetoothConnectivityPage);
    softwareUpdatesPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, SoftwareUpdatesPage);
    wiredNetworkPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, WiredNetworkPage);
    wifiClientPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, WifiClientPage);
    hotSpotPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, HotSpotPage);

    // transitions
    bluetoothConnectivityPageState->addTransition(this, SIGNAL(onEnterSoftwareUpdatesPage()), softwareUpdatesPageState);
    bluetoothConnectivityPageState->addTransition(this, SIGNAL(onEnterWiredNetworkPage()), wiredNetworkPageState);
    bluetoothConnectivityPageState->addTransition(this, SIGNAL(onEnterWifiClientPage()), wifiClientPageState);
    bluetoothConnectivityPageState->addTransition(this, SIGNAL(onEnterHotSpotPage()), hotSpotPageState);

    softwareUpdatesPageState->addTransition(this, SIGNAL(onEnterBluetoothConnectivityPage()), bluetoothConnectivityPageState);
    softwareUpdatesPageState->addTransition(this, SIGNAL(onEnterWiredNetworkPage()), wiredNetworkPageState);
    softwareUpdatesPageState->addTransition(this, SIGNAL(onEnterWifiClientPage()), wifiClientPageState);
    softwareUpdatesPageState->addTransition(this, SIGNAL(onEnterHotSpotPage()), hotSpotPageState);

    wiredNetworkPageState->addTransition(this, SIGNAL(onEnterBluetoothConnectivityPage()), bluetoothConnectivityPageState);
    wiredNetworkPageState->addTransition(this, SIGNAL(onEnterSoftwareUpdatesPage()), softwareUpdatesPageState);
    wiredNetworkPageState->addTransition(this, SIGNAL(onEnterWifiClientPage()), wifiClientPageState);
    wiredNetworkPageState->addTransition(this, SIGNAL(onEnterHotSpotPage()), hotSpotPageState);

    hotSpotPageState->addTransition(this, SIGNAL(onEnterBluetoothConnectivityPage()), bluetoothConnectivityPageState);
    hotSpotPageState->addTransition(this, SIGNAL(onEnterSoftwareUpdatesPage()), softwareUpdatesPageState);
    hotSpotPageState->addTransition(this, SIGNAL(onEnterWiredNetworkPage()), wiredNetworkPageState);
    hotSpotPageState->addTransition(this, SIGNAL(onEnterWifiClientPage()), wifiClientPageState);

    wifiClientPageState->addTransition(this, SIGNAL(onEnterBluetoothConnectivityPage()), bluetoothConnectivityPageState);
    wifiClientPageState->addTransition(this, SIGNAL(onEnterSoftwareUpdatesPage()), softwareUpdatesPageState);
    wifiClientPageState->addTransition(this, SIGNAL(onEnterWiredNetworkPage()), wiredNetworkPageState);
    wifiClientPageState->addTransition(this, SIGNAL(onEnterHotSpotPage()), hotSpotPageState);

    if(availPages[BluetoothConnectivityPage]) {
        machine->setInitialState(bluetoothConnectivityPageState);
        m_currentPage = BluetoothConnectivityPage;
    } else {
        machine->setInitialState(softwareUpdatesPageState);
        m_currentPage = SoftwareUpdatesPage;
    }

    machine->start();
}

BtWifi_SettingsWorkflow::Page BtWifi_SettingsWorkflow::currentPage() const
{
    return m_currentPage;
}

void BtWifi_SettingsWorkflow::setCurrentPage(Page page)
{
    m_currentPage = page;
    emit currentPageChanged(page);
}

void BtWifi_SettingsWorkflow::enterPage(BtWifi_SettingsWorkflow::Page page)
{

    switch (page) {
    case BluetoothConnectivityPage:
        enterBluetoothConnectivityPage();
        break;
    case SoftwareUpdatesPage:
        enterSoftwareUpdatesPage();
        break;
    case WiredNetworkPage:
        enterWiredNetworkPage();
        break;
    case WifiClientPage:
        enterWifiClientPage();
        break;
    case HotSpotPage:
        enterHotSpotPage();
        break;
    default:
        qWarning("No such page");
        break;
    }
}
