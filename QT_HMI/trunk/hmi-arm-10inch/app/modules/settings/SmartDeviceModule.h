#ifndef SETTINGSMODULE_H
#define SETTINGSMODULE_H

#include "AbstractModule.h"

#include <QObject>

namespace QnxCar {
class SmartDeviceModels;
}

class SmartDeviceWorkflow;

class SmartDeviceModule : public QObject, public AbstractModule
{
    Q_OBJECT

    Q_PROPERTY(SmartDeviceWorkflow* workflow READ workflow CONSTANT)
    Q_PROPERTY(QnxCar::SmartDeviceModels* models READ models CONSTANT)

public:
    explicit SmartDeviceModule(QObject *parent = 0);

    QnxCar::SmartDeviceModels *models() const;

    virtual void moduleReset() Q_DECL_OVERRIDE;
    virtual QString navigatorTabName() const Q_DECL_OVERRIDE;

private:
    SmartDeviceWorkflow *workflow() const;

    SmartDeviceWorkflow *m_workflow;
    QnxCar::SmartDeviceModels *m_models;
};

#endif
