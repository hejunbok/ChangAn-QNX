#include "SettingsWorkflow.h"

#include <QState>
#include <QStateMachine>

static const char* CURRENT_PAGE_PROPERTY = "currentPage";

SettingsWorkflow::SettingsWorkflow(QObject* parent)
    : QObject(parent)
    , m_currentPage(SettingsGridPage)
    , m_fluidLevelsDialogVisible(false)
    , m_tractionSystemDialogVisible(false)
    , m_brakingSystemDialogVisible(false)
    , m_powertrainSystemDialogVisible(false)
    , m_electricalSystemDialogVisible(false)
    , m_vehicleStatisticsDialogVisible(false)
{
    initStateMachine();
}

void SettingsWorkflow::leave()
{
    emit onLeave();
}

void SettingsWorkflow::enterSettingsGridPage()
{
    emit onEnterSettingsGridPage();
}

void SettingsWorkflow::enterAudioControlPage()
{
    emit onEnterAudioControlPage();
}

void SettingsWorkflow::enterClimateControlPage()
{
    emit onEnterClimateControlPage();
}

void SettingsWorkflow::enterVirtualMechanicPage()
{
    emit onEnterVirtualMechanicPage();
}

void SettingsWorkflow::enterPersonalizationPage()
{
    emit onEnterPersonalizationPage();
}

void SettingsWorkflow::enterFluidLevelsDialog()
{
    emit onEnterFluidLevelsDialog();
}

void SettingsWorkflow::enterTractionSystemDialog()
{
    emit onEnterTractionSystemDialog();
}

void SettingsWorkflow::enterBrakingSystemDialog()
{
    emit onEnterBrakingSystemDialog();
}

void SettingsWorkflow::enterPowertrainSystemDialog()
{
    emit onEnterPowertrainSystemDialog();
}

void SettingsWorkflow::enterElectricalSystemDialog()
{
    emit onEnterElectricalSystemDialog();
}

void SettingsWorkflow::enterVehicleStatisticsDialog()
{
    emit onEnterVehicleStatisticsDialog();
}

void SettingsWorkflow::initStateMachine()
{
    // states
    QStateMachine *machine = new QStateMachine(this);
    machine->setObjectName(QStringLiteral("SettingsWorkflow"));
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *settingsGridState = new QState(machine);
    settingsGridState->setObjectName(QStringLiteral("SettingsGridPage"));

    QState *subPageState = new QState(machine);

    QState *audioControlPageState = new QState(subPageState);
    audioControlPageState->setObjectName(QStringLiteral("AudioControlPage"));

    QState *climateControlPageState = new QState(subPageState);
    climateControlPageState->setObjectName(QStringLiteral("ClimateControlPage"));

    QState *virtualMechanicPageState = new QState(subPageState);
    virtualMechanicPageState->setObjectName(QStringLiteral("VirtualMechanicPage"));

    QState *personalizationPageState = new QState(subPageState);
    personalizationPageState->setObjectName(QStringLiteral("PersonalizationPage"));

    // properties
    settingsGridState->assignProperty(this, CURRENT_PAGE_PROPERTY, SettingsGridPage);
    audioControlPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, AudioControlPage);
    climateControlPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, ClimateControlPage);
    virtualMechanicPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, VirtualMechanicPage);
    personalizationPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, PersonalizationPage);

    // transitions
    settingsGridState->addTransition(this, SIGNAL(onEnterAudioControlPage()), audioControlPageState);
    settingsGridState->addTransition(this, SIGNAL(onEnterClimateControlPage()), climateControlPageState);
    settingsGridState->addTransition(this, SIGNAL(onEnterVirtualMechanicPage()), virtualMechanicPageState);
    settingsGridState->addTransition(this, SIGNAL(onEnterPersonalizationPage()), personalizationPageState);
    subPageState->addTransition(this, SIGNAL(onEnterSettingsGridPage()), settingsGridState);

    initVirtualMechanicWorkflow(virtualMechanicPageState);

    machine->setInitialState(settingsGridState);
    machine->start();
}

void SettingsWorkflow::initVirtualMechanicWorkflow(QState *virtualMechanicPageState)
{
    // states
    QState *noDialogState = new QState(virtualMechanicPageState);
    noDialogState->setObjectName(QStringLiteral("NoDialog"));

    QState *fluidLevelsDialogState = new QState(virtualMechanicPageState);
    fluidLevelsDialogState->setObjectName(QStringLiteral("FluidLevelsDialog"));

    QState *tractionSystemDialogState = new QState(virtualMechanicPageState);
    tractionSystemDialogState->setObjectName(QStringLiteral("TractionSystemDialog"));

    QState *brakingSystemDialogState = new QState(virtualMechanicPageState);
    brakingSystemDialogState->setObjectName(QStringLiteral("BrakingSystemDialog"));

    QState *powertrainSystemDialogState = new QState(virtualMechanicPageState);
    powertrainSystemDialogState->setObjectName(QStringLiteral("PowertrainSystemDialog"));

    QState *electricalSystemDialogState = new QState(virtualMechanicPageState);
    electricalSystemDialogState->setObjectName(QStringLiteral("ElectricalSystemDialog"));

    QState *vehicleStatisticsDialogState = new QState(virtualMechanicPageState);
    vehicleStatisticsDialogState->setObjectName(QStringLiteral("VehicleStatisticsDialog"));

    // properties
    fluidLevelsDialogState->assignProperty(this, "fluidLevelsDialogVisible", true);
    tractionSystemDialogState->assignProperty(this, "tractionSystemDialogVisible", true);
    brakingSystemDialogState->assignProperty(this, "brakingSystemDialogVisible", true);
    powertrainSystemDialogState->assignProperty(this, "powertrainSystemDialogVisible", true);
    electricalSystemDialogState->assignProperty(this, "electricalSystemDialogVisible", true);
    vehicleStatisticsDialogState->assignProperty(this, "vehicleStatisticsDialogVisible", true);

    // transitions
    noDialogState->addTransition(this, SIGNAL(onEnterFluidLevelsDialog()), fluidLevelsDialogState);
    noDialogState->addTransition(this, SIGNAL(onEnterTractionSystemDialog()), tractionSystemDialogState);
    noDialogState->addTransition(this, SIGNAL(onEnterBrakingSystemDialog()), brakingSystemDialogState);
    noDialogState->addTransition(this, SIGNAL(onEnterPowertrainSystemDialog()), powertrainSystemDialogState);
    noDialogState->addTransition(this, SIGNAL(onEnterElectricalSystemDialog()), electricalSystemDialogState);
    noDialogState->addTransition(this, SIGNAL(onEnterVehicleStatisticsDialog()), vehicleStatisticsDialogState);
    fluidLevelsDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);
    tractionSystemDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);
    brakingSystemDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);
    powertrainSystemDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);
    electricalSystemDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);
    vehicleStatisticsDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);

    virtualMechanicPageState->setInitialState(noDialogState);
}
