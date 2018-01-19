#ifndef SETTINGSMODULE_H
#define SETTINGSMODULE_H

#include "AbstractModule.h"

#include <QObject>

namespace QnxCar {
class SettingsModels;
}

class SettingsWorkflow;
class VehicleStatistics;
class VirtualMechanicLimits;

class SettingsModule : public QObject, public AbstractModule
{
    Q_OBJECT

    Q_PROPERTY(SettingsWorkflow* workflow READ workflow CONSTANT)
    Q_PROPERTY(QnxCar::SettingsModels* models READ models CONSTANT)
    Q_PROPERTY(VirtualMechanicLimits* limits READ limits CONSTANT)
    Q_PROPERTY(VehicleStatistics* vehicleStatistics READ vehicleStatistics CONSTANT)

public:
    explicit SettingsModule(QObject *parent = 0);

    QnxCar::SettingsModels *models() const;

    virtual void moduleReset() Q_DECL_OVERRIDE;
    virtual QString navigatorTabName() const Q_DECL_OVERRIDE;

private:
    SettingsWorkflow *workflow() const;
    VirtualMechanicLimits *limits() const;
    VehicleStatistics *vehicleStatistics() const;

    SettingsWorkflow *m_workflow;
    QnxCar::SettingsModels *m_models;
    VirtualMechanicLimits *m_limits;
    VehicleStatistics *m_vehicleStatistics;
};

#endif
