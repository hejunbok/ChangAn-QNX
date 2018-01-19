#ifndef NAVIGATIONWORKFLOW_H
#define NAVIGATIONWORKFLOW_H

#include <QObject>

#include "util/Util.h"

class QHistoryState;
class QState;

class NavigationWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(NavigationWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(NavigationWorkflow::SearchSubpage currentSearchSubpage READ currentSearchSubpage WRITE setCurrentSearchSubpage NOTIFY currentSearchSubpageChanged)
    Q_PROPERTY(bool favouriteExtended READ favouriteExtended WRITE setFavouriteExtended NOTIFY favouriteExtendedChanged)
    Q_PROPERTY(bool sheetMenuExtended READ sheetMenuExtended WRITE setSheetMenuExtended NOTIFY sheetMenuExtendedChanged)
    Q_ENUMS(Page)
    Q_ENUMS(SearchSubpage)

public:
    enum Page {
        /// Overview page
        NavigationGridPage,
        SearchDestinationPage,
        BrowseMapPage
    };

    enum SearchSubpage {
        NoSearchSubpage,
        SearchTypeSubpage,
        SearchPOISubpage,
        SearchCitySubpage,
        SearchStreetSubpage,
        SearchNumberSubpage,
        SearchAddressSubpage
    };

    explicit NavigationWorkflow(QObject *parent = 0);

    PROPERTY_WITH_NOTIFY(NavigationWorkflow::Page, currentPage, setCurrentPage, currentPageChanged)
    PROPERTY_WITH_NOTIFY(NavigationWorkflow::SearchSubpage, currentSearchSubpage, setCurrentSearchSubpage, currentSearchSubpageChanged)
    PROPERTY_WITH_NOTIFY(bool, sheetMenuExtended, setSheetMenuExtended, sheetMenuExtendedChanged)
    PROPERTY_WITH_NOTIFY(bool, favouriteExtended, setFavouriteExtended, favouriteExtendedChanged)

public Q_SLOTS:
    void enterNavigationGridPage();

    // top-level pages
    void enterSearchDestinationPage();
    void enterBrowseMapPage();
    void enterNavigationSettingsPage();

    void leaveSearchTypeSubpage(int loc);
    void enterSearchStreetSubpage();
    void enterSearchNumberSubpage();
    void enterSearchAddressSubpage();

    void back();
    /**
     * @brief Returns the search destination page substate back to the select search type state.
     */
    void backToSearchTypeSubpage();

Q_SIGNALS:
    void onEnterNavigationGridPage();

    // top-level pages
    void onEnterSearchDestinationPage();
    void onEnterBrowseMapPage();
    void onEnterNavigationSettingsPage();

    void onEnterSearchPOISubpage();
    void onEnterSearchCitySubpage();
    void onEnterSearchStreetSubpage();
    void onEnterSearchNumberSubpage();
    void onEnterSearchAddressSubpage();

    void onBack();

private:
    void initStateMachine();
    QHistoryState *initSearchDestinationState(QState *searchState);
};

#endif
