#include "CommunicationsWorkflow.h"
#include <QStateMachine>

static const char *CURRENT_PAGE_PROPERTY = "currentPage";
static const char *CURRENT_OVERLAY_PROPERTY = "currentOverlay";

CommunicationsWorkflow::CommunicationsWorkflow(QObject *parent)
    : QObject(parent)
    , m_currentPage(GridPage)
    , m_currentOverlay(NoOverlay)
    , m_sheetMenuExtended(false)
{
    initStateMachine();
}

void CommunicationsWorkflow::initStateMachine()
{
    QStateMachine *machine = new QStateMachine(QState::ParallelStates, this);
    machine->setObjectName(QStringLiteral("CommunicationsWorkflow"));
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    // 'main' and 'overlay' are the two parallel (aggregate) states of
    // 'machine'. We use 'overlay' to show the call UI without switching away
    // from the current page. We return to current page if there's no active
    // overlay.
    QState *main = new QState(machine);
    main->setObjectName(QStringLiteral("CommunicationsMainState"));
    QState *overlay = new QState(machine);
    overlay->setObjectName(QStringLiteral("CommunicationsOverlayState"));

    QState *grid = new QState(main);
    grid->setObjectName(QStringLiteral("CommunicationsGridState"));

    // subPage is another aggregate state and does not represent a specific
    // page on its own:
    QState *subPage = new QState(main);
    QState *sub2Page = new QState(subPage);

    QState *callOverlay = new QState(overlay);
    callOverlay->setObjectName(QStringLiteral("CommunicationsCallOverlayState"));
    QState *noOverlay = new QState(overlay);
    noOverlay->setObjectName(QStringLiteral("CommunicationsNoOverlayState"));
    QState *dialPad = new QState(sub2Page);
    dialPad->setObjectName(QStringLiteral("CommunicationsDialPadState"));
    QState *contactSearchPage = new QState(sub2Page);
    contactSearchPage->setObjectName(QStringLiteral("CommunicationsContactSearchPageState"));

    QState *smartWatch = new QState(subPage);
    smartWatch->setObjectName(QStringLiteral("CommunicationsSmartWatchPageState"));
    QState *smartGlass = new QState(subPage);
    smartGlass->setObjectName(QStringLiteral("CommunicationsSmartGlassPageState"));
    QState *blueTooth = new QState(subPage);
    blueTooth->setObjectName(QStringLiteral("CommunicationsBlueToothPageState"));
    QState *btwifiSettings = new QState(subPage);
    btwifiSettings->setObjectName(QStringLiteral("CommunicationsBtWifiSettingsPageState"));


    grid->assignProperty(this, CURRENT_PAGE_PROPERTY, GridPage);
    callOverlay->assignProperty(this, CURRENT_OVERLAY_PROPERTY, CallOverlay);
    noOverlay->assignProperty(this, CURRENT_OVERLAY_PROPERTY, NoOverlay);
    smartWatch->assignProperty(this, CURRENT_PAGE_PROPERTY, SmartWatchPage);
    smartGlass->assignProperty(this, CURRENT_PAGE_PROPERTY, SmartGlassPage);
    blueTooth->assignProperty(this, CURRENT_PAGE_PROPERTY, BlueToothPage);
    dialPad->assignProperty(this, CURRENT_PAGE_PROPERTY, DialPadPage);
    contactSearchPage->assignProperty(this, CURRENT_PAGE_PROPERTY, ContactSearchPage);
    btwifiSettings->assignProperty(this, CURRENT_PAGE_PROPERTY, BtWifiSettings);

    callOverlay->addTransition(this, SIGNAL(onLeaveCallPage()), noOverlay);
    noOverlay->addTransition(this, SIGNAL(onEnterCallPage()), callOverlay);
    grid->addTransition(this, SIGNAL(onEnterSmartWatchPage()), smartWatch);
    grid->addTransition(this, SIGNAL(onEnterSmartGlassPage()), smartGlass);
    grid->addTransition(this, SIGNAL(onEnterBlueToothPage()), blueTooth);
    grid->addTransition(this, SIGNAL(onEnterBtWifiSettingPage()), btwifiSettings);
    subPage->addTransition(this, SIGNAL(onEnterGridPage()), grid);
    subPage->addTransition(this, SIGNAL(onEnterDialPadPage()), dialPad);
    subPage->addTransition(this, SIGNAL(onEnterContactSearchPage()), contactSearchPage);
    contactSearchPage->addTransition(this, SIGNAL(onEnterDialPadPage()), dialPad);
    dialPad->addTransition(this, SIGNAL(onEnterGridPage()), blueTooth);

    main->setInitialState(grid);
    overlay->setInitialState(noOverlay);
    machine->start();



}

void CommunicationsWorkflow::enterGridPage()
{
    Q_EMIT onEnterGridPage();
}

void CommunicationsWorkflow::enterDialPadPage()
{
    Q_EMIT onEnterDialPadPage();
}

void CommunicationsWorkflow::enterContactSearchPage()
{
    Q_EMIT onEnterContactSearchPage();
}

void CommunicationsWorkflow::enterCallPage()
{
    Q_EMIT onEnterCallPage();
}

void CommunicationsWorkflow::leaveCallPage()
{
    Q_EMIT onLeaveCallPage();
}

void CommunicationsWorkflow::enterSmartGridpage()
{
    Q_EMIT onEnterSmartGridpage();
}

void CommunicationsWorkflow::enterCarPlayPage()
{
    Q_EMIT onEnterCarPlayPage();
}

void CommunicationsWorkflow::enterMirrorLinkPage()
{
    Q_EMIT onEnterMirrorLinkPage();
}

void CommunicationsWorkflow::enterSmartWatchPage()
{
    Q_EMIT onEnterSmartWatchPage();
}

void CommunicationsWorkflow::enterSmartGlassPage()
{
    Q_EMIT onEnterSmartGlassPage();
}

void CommunicationsWorkflow::enterBlueToothPage()
{
    Q_EMIT onEnterBlueToothPage();
}

void CommunicationsWorkflow::leaveSmartGridpage()
{
    Q_EMIT onLeaveSmartGridpage();
}

void CommunicationsWorkflow::leaveCarPlayPage()
{
    Q_EMIT onLeaveCarPlayPage();
}

void CommunicationsWorkflow::leaveMirrorLinkPage()
{
    Q_EMIT onLeaveMirrorLinkPage();
}

void CommunicationsWorkflow::leaveSmartWatchPage()
{
    Q_EMIT onLeaveSmartWatchPage();
}

void CommunicationsWorkflow::leaveSmartGlassPage()
{
    Q_EMIT onLeaveSmartGlassPage();
}

void CommunicationsWorkflow::enterBtWifiSettingPage()
{
    Q_EMIT onEnterBtWifiSettingPage();
}

void CommunicationsWorkflow::leaveBtWifiSettingPage()
{
    Q_EMIT onLeaveBtWifiSettingPage();
}
