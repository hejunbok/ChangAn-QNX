#include "NavigationWorkflow.h"

#include <QHistoryState>
#include <QStateMachine>

static const char* CURRENT_PAGE_PROPERTY = "currentPage";
static const char* CURRENT_SEARCHSUBPAGE_PROPERTY = "currentSearchSubpage";

NavigationWorkflow::NavigationWorkflow(QObject *parent)
    : QObject(parent)
    , m_currentPage(NavigationGridPage)
    , m_currentSearchSubpage(NoSearchSubpage)
    , m_sheetMenuExtended(false)
    , m_favouriteExtended(false)
{
    initStateMachine();
}

void NavigationWorkflow::initStateMachine()
{
    // states
    QStateMachine *machine = new QStateMachine(this);
    machine->setObjectName(QStringLiteral("NavigationWorkflow"));
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *navigationGridState = new QState(machine);
    navigationGridState->setObjectName(QStringLiteral("NavigationGridPage"));

    QState *subPageState = new QState(machine);

    QState *searchDestinationPageState = new QState(subPageState);
    searchDestinationPageState->setObjectName(QStringLiteral("SearchDestinationPage"));

    QHistoryState *searchPageHistoryState = initSearchDestinationState(searchDestinationPageState);

    QState *browseMapPageState = new QState(subPageState);
    browseMapPageState->setObjectName(QStringLiteral("BrowseMapPage"));

    QState *navigationSettingsPageState = new QState(subPageState);
    navigationSettingsPageState->setObjectName(QStringLiteral("NavigationSettingsPage"));

    // properties
    navigationGridState->assignProperty(this, CURRENT_PAGE_PROPERTY, NavigationGridPage);
    searchDestinationPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, SearchDestinationPage);
    browseMapPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, BrowseMapPage);

    // transitions
    navigationGridState->addTransition(this, SIGNAL(onEnterSearchDestinationPage()), searchPageHistoryState);
    navigationGridState->addTransition(this, SIGNAL(onEnterBrowseMapPage()), browseMapPageState);
    navigationGridState->addTransition(this, SIGNAL(onEnterNavigationSettingsPage()), navigationSettingsPageState);
    subPageState->addTransition(this, SIGNAL(onEnterNavigationGridPage()), navigationGridState);

    machine->setInitialState(navigationGridState);
    machine->start();
}

QHistoryState *NavigationWorkflow::initSearchDestinationState(QState *searchState)
{
    QState *searchTypeSubpageState = new QState(searchState);
    searchTypeSubpageState->setObjectName(QStringLiteral("SearchTypeSubpage"));

    QState *searchPOISubpageState = new QState(searchState);
    searchPOISubpageState->setObjectName(QStringLiteral("SearchPOISubpage"));

    QState *searchCitySubpageState = new QState(searchState);
    searchCitySubpageState->setObjectName(QStringLiteral("SearchCitySubpage"));

    QState *searchStreetSubpageState = new QState(searchState);
    searchStreetSubpageState->setObjectName(QStringLiteral("SearchStreetSubpage"));

    QState *searchNumberSubpageState = new QState(searchState);
    searchNumberSubpageState->setObjectName(QStringLiteral("SearchNumberSubpage"));

    QState *searchAddressSubpageState = new QState(searchState);
    searchAddressSubpageState->setObjectName(QStringLiteral("SearchAddressSubpage"));

    //properties
    searchTypeSubpageState->assignProperty(this, CURRENT_SEARCHSUBPAGE_PROPERTY, SearchTypeSubpage);
    searchPOISubpageState->assignProperty(this, CURRENT_SEARCHSUBPAGE_PROPERTY, SearchPOISubpage);
    searchCitySubpageState->assignProperty(this, CURRENT_SEARCHSUBPAGE_PROPERTY, SearchCitySubpage);
    searchStreetSubpageState->assignProperty(this, CURRENT_SEARCHSUBPAGE_PROPERTY, SearchStreetSubpage);
    searchNumberSubpageState->assignProperty(this, CURRENT_SEARCHSUBPAGE_PROPERTY, SearchNumberSubpage);
    searchAddressSubpageState->assignProperty(this, CURRENT_SEARCHSUBPAGE_PROPERTY, SearchAddressSubpage);

    //transitions
    searchTypeSubpageState->addTransition(this, SIGNAL(onEnterSearchAddressSubpage()), searchAddressSubpageState);
    searchTypeSubpageState->addTransition(this, SIGNAL(onEnterSearchCitySubpage()), searchCitySubpageState);
    searchTypeSubpageState->addTransition(this, SIGNAL(onEnterSearchPOISubpage()), searchPOISubpageState);

    searchCitySubpageState->addTransition(this, SIGNAL(onEnterSearchStreetSubpage()), searchStreetSubpageState);
    searchStreetSubpageState->addTransition(this, SIGNAL(onEnterSearchNumberSubpage()), searchNumberSubpageState);

    searchPOISubpageState->addTransition(this, SIGNAL(onBack()), searchTypeSubpageState);
    searchCitySubpageState->addTransition(this, SIGNAL(onBack()), searchTypeSubpageState);
    searchNumberSubpageState->addTransition(this, SIGNAL(onBack()), searchStreetSubpageState);
    searchStreetSubpageState->addTransition(this, SIGNAL(onBack()), searchCitySubpageState);
    searchAddressSubpageState->addTransition(this, SIGNAL(onBack()), searchTypeSubpageState);

    QHistoryState *historyState = new QHistoryState(searchState);
    historyState->setDefaultState(searchTypeSubpageState);
    return historyState;
}

void NavigationWorkflow::enterNavigationGridPage()
{
    emit onEnterNavigationGridPage();
}

void NavigationWorkflow::enterSearchDestinationPage()
{
    emit onEnterSearchDestinationPage();
}

void NavigationWorkflow::enterBrowseMapPage()
{
    emit onEnterBrowseMapPage();
}

void NavigationWorkflow::enterNavigationSettingsPage()
{
    emit onEnterNavigationSettingsPage();
}

void NavigationWorkflow::leaveSearchTypeSubpage(int loc)
{
    if(loc == 0)
        emit onEnterSearchCitySubpage();
    else
        emit onEnterSearchPOISubpage();
}

void NavigationWorkflow::enterSearchAddressSubpage()
{
    emit onEnterSearchAddressSubpage();
}

void NavigationWorkflow::enterSearchStreetSubpage()
{
    emit onEnterSearchStreetSubpage();
}

void NavigationWorkflow::enterSearchNumberSubpage()
{
    emit onEnterSearchNumberSubpage();
}

void NavigationWorkflow::back()
{
    emit onBack();
}

void NavigationWorkflow::backToSearchTypeSubpage(){
    int count = 0;
    if(m_currentSearchSubpage != NavigationWorkflow::NoSearchSubpage){
        while (m_currentSearchSubpage != NavigationWorkflow::SearchTypeSubpage) {
            count++;
            if(count >= 100){
                qWarning()<<Q_FUNC_INFO<<"Exiting from the while loop, Navigation workflow is in a unexpected state";
                return;
            }
            //This shifts the state back by one towards the root according to the defined state transitions
            back();
        }
    }
}
