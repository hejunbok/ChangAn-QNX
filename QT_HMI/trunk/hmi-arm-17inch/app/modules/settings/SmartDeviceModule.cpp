#include "SmartDeviceModule.h"

#include "qnxcar/SettingsModels.h"
#include "qnxcar/ProfileDetailsObject.h"
#include "qnxcar/DatabaseManager.h"

#include "SmartDeviceWorkflow.h"

#include <qqml.h>

SmartDeviceModule::SmartDeviceModule(QObject *parent)
    : QObject(parent)
    , m_workflow(new SmartDeviceWorkflow(this))
    , m_models(new QnxCar::SmartDeviceModels(this))
{
    qmlRegisterUncreatableType<QnxCar::BluetoothDevicesModel>("com.qnx.car.core", 2, 0, "BluetoothDevicesModel", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::AvatarListModel>("com.qnx.car.ui", 2, 0, "AvatarListModel", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::ThemeListModel>("com.qnx.car.ui", 2, 0, "ThemeListModel", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<SmartDeviceModule>("com.qnx.car.ui", 2, 0, "SmartDeviceWorkflow", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::SettingsModels>("com.qnx.car.ui", 2, 0, "SmartDeviceModels", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::ProfileDetailsObject>("com.qnx.car.ui", 2, 0, "ProfileDetailsObject", QStringLiteral("Access to object"));

    //m_profileModel->setDatabase(DatabaseManager::database(QStringLiteral("personalization.db")));
}

void SmartDeviceModule::moduleReset()
{
    m_workflow->enterWatchDialog();
}

SmartDeviceModule *SettingsModule::workflow() const
{
    return m_workflow;
}

QnxCar::SettingsModels *SettingsModule::models() const
{
    return m_models;
}

QString SmartDeviceModule::navigatorTabName() const
{
    return QStringLiteral("Settings");
}


