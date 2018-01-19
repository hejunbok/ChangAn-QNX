#include "SettingsModule.h"

#include "qnxcar/SettingsModels.h"
#include "qnxcar/ProfileDetailsObject.h"
#include "qnxcar/DatabaseManager.h"
#include "services/BluetoothDevicesModel.h"
#include "services/WifiAPModel.h"

#include "SettingsWorkflow.h"
#include "VehicleStatistics.h"
#include "VirtualMechanicLimits.h"

#include <qqml.h>

SettingsModule::SettingsModule(QObject *parent)
    : QObject(parent)
    , m_workflow(new SettingsWorkflow(this))
    , m_models(new QnxCar::SettingsModels(this))
    , m_limits(new VirtualMechanicLimits(this))
    , m_vehicleStatistics(new VehicleStatistics(this))
{
    qmlRegisterUncreatableType<QnxCar::BluetoothDevicesModel>("com.qnx.car.core", 2, 0, "BluetoothDevicesModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::WifiAPModel>("com.qnx.car.core", 2, 0, "WifiAPModel", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::AvatarListModel>("com.qnx.car.ui", 2, 0, "AvatarListModel", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::ThemeListModel>("com.qnx.car.ui", 2, 0, "ThemeListModel", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<SettingsWorkflow>("com.qnx.car.ui", 2, 0, "SettingsWorkflow", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::SettingsModels>("com.qnx.car.ui", 2, 0, "SettingsModels", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::ProfileDetailsObject>("com.qnx.car.ui", 2, 0, "ProfileDetailsObject", QStringLiteral("Access to object"));

    qmlRegisterType<VirtualMechanicLimits>();
    qmlRegisterType<VehicleStatistics>();

    //m_profileModel->setDatabase(DatabaseManager::database(QStringLiteral("personalization.db")));
}

void SettingsModule::moduleReset()
{
    m_workflow->enterSettingsGridPage();
}

SettingsWorkflow *SettingsModule::workflow() const
{
    return m_workflow;
}

QnxCar::SettingsModels *SettingsModule::models() const
{
    return m_models;
}

VirtualMechanicLimits *SettingsModule::limits() const
{
    return m_limits;
}

VehicleStatistics *SettingsModule::vehicleStatistics() const
{
    return m_vehicleStatistics;
}

QString SettingsModule::navigatorTabName() const
{
    return QStringLiteral("Settings");
}
