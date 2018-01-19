#include "SmartDeviceWorkflow.h"

#include <QState>
#include <QStateMachine>

static const char* CURRENT_PAGE_PROPERTY = "currentPage";

SmartDeviceWorkflow::SmartDeviceWorkflow(QObject* parent)
    : QObject(parent)
    , m_currentPage(PersonalSettingPage)
    , m_watchDialogVisible(false)
    , m_glassDialogVisible(false)
{
    initStateMachine();
}

void SmartDeviceWorkflow::leave()
{
    emit onLeave();
}

void SmartDeviceWorkflow::enterPersonalSettingPage()
{
    emit onEnterPersonalSettingPage();
}

void SmartDeviceWorkflow::enterWatchDialog()
{
	emit onEnterWatchDialog();
}

void SmartDeviceWorkflow::enterGlassDialog()
{
    emit onEnterGlassDialog();
}

void SmartDeviceWorkflow::initStateMachine()
{
    // states
    QStateMachine *machine = new QStateMachine(this);
    machine->setObjectName(QStringLiteral("SmartDeviceWorkflow"));
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *settingsGridState = new QState(machine);
    settingsGridState->setObjectName(QStringLiteral("SettingsGridPage"));

    QState *subPageState = new QState(machine);

    QState *watchpageState = new QState(subPageState);
    watchpageState->setObjectName(QStringLiteral("WatchControlPage"));

    QState *glasspageState = new QState(subPageState);
    glasspageState->setObjectName(QStringLiteral("GlassControlPage"));

    // properties
    settingsGridState->assignProperty(this, CURRENT_PAGE_PROPERTY, PersonalSettingPage);
    watchpageState->assignProperty(this, CURRENT_PAGE_PROPERTY, WatchControlPage);
    glasspageState->assignProperty(this, CURRENT_PAGE_PROPERTY, GlassControlPage);

    settingsGridState->addTransition(this, SIGNAL(onEnterWatchDialog()), watchpageState);
    settingsGridState->addTransition(this, SIGNAL(onEnterGlassDialog()), glasspageState);
    subPageState->addTransition(this, SIGNAL(onEnterPersonalSettingPage()), settingsGridState);

    initSmartDeviceWorkflow(settingsGridState);

    machine->setInitialState(settingsGridState);
    machine->start();
}

void SmartDeviceWorkflow::initSmartDeviceWorkflow(QState *virtualMechanicPageState)
{
    // states
    QState *noDialogState = new QState(virtualMechanicPageState);
    noDialogState->setObjectName(QStringLiteral("NoDialog"));

    QState *watchDialogState = new QState(virtualMechanicPageState);
    watchDialogState->setObjectName(QStringLiteral("WatchDialogState"));

    QState *glassDialogState = new QState(virtualMechanicPageState);
    glassDialogState->setObjectName(QStringLiteral("GlassDialogState"));

    // properties
    watchDialogState->assignProperty(this, "watchDialogVisible", true);
    glassDialogState->assignProperty(this, "glassDialogDialogVisible", true);

    // transitions
    noDialogState->addTransition(this, SIGNAL(onEnterWatchDialog()), watchDialogState);
    noDialogState->addTransition(this, SIGNAL(onEnterGlassDialog()), glassDialogState);

    watchDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);
    glassDialogState->addTransition(this, SIGNAL(onLeave()), noDialogState);

    virtualMechanicPageState->setInitialState(noDialogState);
}
