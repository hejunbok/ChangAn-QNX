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

    QState *dialPad = new QState(subPage);
    QState *contactSearchPage = new QState(subPage);

    QState *callOverlay = new QState(overlay);
    QState *noOverlay = new QState(overlay);

    dialPad->setObjectName(QStringLiteral("CommunicationsDialPadState"));
    callOverlay->setObjectName(QStringLiteral("CommunicationsCallOverlayState"));
    noOverlay->setObjectName(QStringLiteral("CommunicationsNoOverlayState"));
    contactSearchPage->setObjectName(QStringLiteral("CommunicationsContactSearchPageState"));

    grid->assignProperty(this, CURRENT_PAGE_PROPERTY, GridPage);
    dialPad->assignProperty(this, CURRENT_PAGE_PROPERTY, DialPadPage);
    callOverlay->assignProperty(this, CURRENT_OVERLAY_PROPERTY, CallOverlay);
    noOverlay->assignProperty(this, CURRENT_OVERLAY_PROPERTY, NoOverlay);
    contactSearchPage->assignProperty(this, CURRENT_PAGE_PROPERTY, ContactSearchPage);

    grid->addTransition(this, SIGNAL(onEnterDialPadPage()), dialPad);
    grid->addTransition(this, SIGNAL(onEnterContactSearchPage()), contactSearchPage);
    contactSearchPage->addTransition(this, SIGNAL(onEnterDialPadPage()), dialPad);
    subPage->addTransition(this, SIGNAL(onEnterGridPage()), grid);
    callOverlay->addTransition(this, SIGNAL(onLeaveCallPage()), noOverlay);
    noOverlay->addTransition(this, SIGNAL(onEnterCallPage()), callOverlay);

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
