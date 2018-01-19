#include "Core.h"
#include "NavigationModule.h"
#include "NavigationWorkflow.h"
#include "NavigationController.h"
#include "qnxcar/LocationCategoryModel.h"
#include "qnxcar/LocationCategoryFilterModel.h"
#include "qnxcar/LocationObject.h"

#include "qnxcar/DatabaseManager.h"
#include "qnxcar/FavouriteModel.h"
#include "qnxcar/Location.h"
#include "qnxcar/LocationModel.h"
#include "qnxcar/ManeuversModel.h"
#include "qnxcar/Navigation.h"
#include "qnxcar/NavigationHistoryModel.h"
#include "qnxcar/NavigationSettingsModel.h"
#include "pps/ControlCommand.h"

#include <qqml.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QJsonObject>

NavigationModule::NavigationModule(QObject *parent)
    : QObject(parent)
    , m_workflow(new NavigationWorkflow(this))
    , m_locationCategoryModel(new QnxCar::LocationCategoryModel(this))
    , m_locationModel(new QnxCar::LocationModel(this))
    , m_poiSearchModel(new QnxCar::LocationModel(this))
    , m_citySearchModel(new QnxCar::LocationModel(this))
    , m_streetSearchModel(new QnxCar::LocationModel(this))
    , m_addressSearchModel(new QnxCar::LocationModel(this))
    , m_numberSearchModel(new QnxCar::LocationModel(this))
    , m_navigation(new QnxCar::Navigation(this))
    , m_favouriteModel(new QnxCar::FavouriteModel(this))
    , m_historyModel(new QnxCar::NavigationHistoryModel(this))
    , m_currentDestination(new QnxCar::LocationObject)
    , m_settingModel(new QnxCar::NavigationSettingsModel(this))
{
    qmlRegisterUncreatableType<NavigationWorkflow>("com.qnx.car.ui", 2, 0,
        "NavigationWorkflow", QStringLiteral("Access to enums"));

    qmlRegisterUncreatableType<QnxCar::LocationCategoryModel>("com.qnx.car.ui", 2, 0,
        "LocationCategoryModel", QStringLiteral("Access to objects"));

    qmlRegisterType<QnxCar::LocationCategoryFilterModel>("com.qnx.car.ui", 2, 0, "LocationCategoryFilterModel");

    qmlRegisterUncreatableType<QnxCar::LocationModel>("com.qnx.car.ui", 2, 0,
        "LocationModel", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::ManeuversModel>("com.qnx.car.ui", 2, 0,
        "ManeuversModel", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::Navigation>("com.qnx.car.ui", 2, 0,
        "Navigation", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::FavouriteModel>("com.qnx.car.ui", 2, 0,
        "FavouriteModel", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::NavigationHistoryModel>("com.qnx.car.ui", 2, 0,
        "NavigationHistoryModel", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::LocationObject>("com.qnx.car.ui", 2, 0,
        "LocationObject", QStringLiteral("Access to object"));

    qmlRegisterUncreatableType<QnxCar::NavigationSettingsModel>("com.qnx.car.ui", 2, 0,
        "NavigationSettingsModel", QStringLiteral("Access to enums"));

    qmlRegisterUncreatableType<QnxCar::ControlCommand>("com.qnx.car.ui", 2, 0,
        "ControlCommand", QStringLiteral("Access to object"));

    const QSqlDatabase navigationDb = QnxCar::DatabaseManager::database(QStringLiteral("navigation.db"));
    const QSqlDatabase personalizationDb = QnxCar::DatabaseManager::database(QStringLiteral("personalization.db"));

    m_locationCategoryModel->setDatabase(navigationDb);
    m_locationModel->setDatabase(navigationDb);
    m_citySearchModel->setDatabase(navigationDb);
    m_streetSearchModel->setDatabase(navigationDb);
    m_numberSearchModel->setDatabase(navigationDb);
    m_addressSearchModel->setDatabase(navigationDb);
    m_poiSearchModel->setDatabase(navigationDb);
    m_favouriteModel->setDatabase(personalizationDb);
    m_historyModel->setDatabase(personalizationDb);
    m_settingModel->setDatabase(personalizationDb);

    connect(m_navigation, SIGNAL(pointsOfInterestResultsReceived()), m_locationModel, SLOT(update()));
    connect(m_navigation, SIGNAL(destinationChanged(QnxCar::Location)), m_currentDestination, SLOT(setLocation(QnxCar::Location)));
    m_currentDestination->setLocation(m_navigation->destination());

    //clear any initial search data in navigation.db by performing an empty search
    QnxCar::ControlCommand *command = m_navigation->search(QString(), QString(), QString(), QString());
    connect(command, &QnxCar::ControlCommand::finished, m_citySearchModel, &QnxCar::LocationModel::update);
    connect(command, &QnxCar::ControlCommand::finished, m_streetSearchModel, &QnxCar::LocationModel::update);
    connect(command, &QnxCar::ControlCommand::finished, m_numberSearchModel, &QnxCar::LocationModel::update);
    connect(command, &QnxCar::ControlCommand::finished, m_addressSearchModel, &QnxCar::LocationModel::update);

    #ifdef Q_OS_QNX
        Core::self()->navigationController()->setNavigationVisible(true);
    #endif
}

void NavigationModule::moduleReset()
{
    m_workflow->setSheetMenuExtended(false);
    m_workflow->enterNavigationGridPage();
}

void NavigationModule::moduleSelected()
{

}

void NavigationModule::moduleDeselected()
{

}


QnxCar::LocationCategoryModel *NavigationModule::locationCategoryModel() const
{
    return m_locationCategoryModel;
}

QnxCar::FavouriteModel *NavigationModule::favouriteModel() const
{
    return m_favouriteModel;
}

QnxCar::NavigationHistoryModel *NavigationModule::historyModel() const
{
    return m_historyModel;
}

NavigationWorkflow *NavigationModule::workflow() const
{
    return m_workflow;
}

QnxCar::LocationModel *NavigationModule::locationModel() const
{
    return m_locationModel;
}

QnxCar::LocationModel *NavigationModule::poiSearchModel() const
{
    return m_poiSearchModel;
}

QnxCar::LocationModel *NavigationModule::citySearchModel() const
{
    return m_citySearchModel;
}

QnxCar::LocationModel *NavigationModule::streetSearchModel() const
{
    return m_streetSearchModel;
}

QnxCar::LocationModel *NavigationModule::numberSearchModel() const
{
    return m_numberSearchModel;
}

QnxCar::LocationModel *NavigationModule::addressSearchModel() const
{
    return m_addressSearchModel;
}

QnxCar::Navigation *NavigationModule::navigation() const
{
    return m_navigation;
}

QnxCar::NavigationSettingsModel *NavigationModule::settingModel() const
{
    return m_settingModel;
}

void NavigationModule::setPOISearchString(const QString &poi)
{
    if (m_poiSearchString != poi) {
        m_poiSearchString = poi;
        emit poiSearchStringChanged(poi);
    }
}

void NavigationModule::setCitySearchString(const QString &city)
{
    if (m_citySearchString != city) {
        m_citySearchString = city;
        emit citySearchStringChanged(city);
    }
}

void NavigationModule::setStreetSearchString(const QString &street)
{
    if (m_streetSearchString != street) {
        m_streetSearchString = street;
        emit streetSearchStringChanged(street);
    }
}

void NavigationModule::setNumberSearchString(const QString &number)
{
    if (m_numberSearchString != number) {
        m_numberSearchString = number;
        emit numberSearchStringChanged(number);
    }
}

void NavigationModule::setAddressSearchString(const QString &address)
{
    if (m_addressSearchString != address) {
        m_addressSearchString = address;
        emit addressSearchStringChanged(address);
    }
}

void NavigationModule::searchPOI(const QString &name)
{
    const NavigationWorkflow::SearchSubpage subpage = m_workflow->currentSearchSubpage();
    Q_ASSERT(subpage != NavigationWorkflow::NoSearchSubpage);
    QnxCar::ControlCommand *command = m_navigation->searchPointsOfInterest(name);

    QnxCar::LocationModel *targetModel = 0;
    targetModel = m_poiSearchModel;
    connect(command, SIGNAL(finished(QnxCar::ControlCommand*)), targetModel, SLOT(update()));
}

void NavigationModule::search(const QString &country, const QString &city, const QString &street, const QString &number)
{
    const NavigationWorkflow::SearchSubpage subpage = m_workflow->currentSearchSubpage();
    Q_ASSERT(subpage != NavigationWorkflow::NoSearchSubpage);
    QnxCar::ControlCommand *command = m_navigation->search(country, city, street, number);

    QnxCar::LocationModel *targetModel = 0;
    switch (subpage) {
    case NavigationWorkflow::NoSearchSubpage:
        break;
    case NavigationWorkflow::SearchCitySubpage:
        targetModel = m_citySearchModel;
        break;
    case NavigationWorkflow::SearchStreetSubpage:
        targetModel = m_streetSearchModel;
        break;
    case NavigationWorkflow::SearchNumberSubpage:
        targetModel = m_numberSearchModel;
        break;
    case NavigationWorkflow::SearchAddressSubpage:
        targetModel = m_addressSearchModel;
        break;
     default:
        break;
    }
    Q_ASSERT(targetModel);
    connect(command, SIGNAL(finished(QnxCar::ControlCommand*)), targetModel, SLOT(update()));
}

void NavigationModule::addFavourite(int historyRow)
{
    const QModelIndex &index = m_historyModel->index(historyRow, 0);
    QnxCar::Location l = index.data(QnxCar::NavigationHistoryModel::LocationRole).value<QnxCar::Location>();
    m_favouriteModel->addToFavourites(l);
}

void NavigationModule::deleteFavourite(int favouriteRow)
{
    const QModelIndex &index = m_favouriteModel->index(favouriteRow, 0);
    QnxCar::Location l = index.data(QnxCar::FavouriteModel::LocationRole).value<QnxCar::Location>();
    m_favouriteModel->deleteFromFavourites(l);
}

void NavigationModule::navigateTo(const QModelIndex &index, int role)
{
    const QnxCar::Location l = index.data(role).value<QnxCar::Location>();

    Q_ASSERT(l.isValid());

    m_historyModel->addToHistory(l);
    m_navigation->navigateTo(l);
}

void NavigationModule::navigateToLocation(int locationRow)
{
    navigateTo(m_locationModel->index(locationRow, 0), QnxCar::LocationModel::LocationRole);
}

void NavigationModule::navigateToPOI(int poiRow)
{
    navigateTo(m_poiSearchModel->index(poiRow, 0), QnxCar::LocationModel::LocationRole);
}

void NavigationModule::navigateToFavourite(int favouriteRow)
{
    navigateTo(m_favouriteModel->index(favouriteRow, 0), QnxCar::FavouriteModel::LocationRole);
}

void NavigationModule::navigateToHistory(int historyRow)
{
    navigateTo(m_historyModel->index(historyRow, 0), QnxCar::NavigationHistoryModel::LocationRole);
}

void NavigationModule::navigateToSearchResult(int locationRow)
{
    navigateTo(m_numberSearchModel->index(locationRow, 0), QnxCar::LocationModel::LocationRole);
}

void NavigationModule::navigateToAddressResult(int locationRow)
{
    navigateTo(m_addressSearchModel->index(locationRow, 0), QnxCar::LocationModel::LocationRole);
}


QnxCar::LocationObject *NavigationModule::currentDestination() const
{
    return m_currentDestination;
}

QString NavigationModule::navigatorTabName() const
{
    return QStringLiteral("navigation");
}

QString NavigationModule::poiSearchString() const
{
    return m_poiSearchString;
}

QString NavigationModule::citySearchString() const
{
    return m_citySearchString;
}

QString NavigationModule::streetSearchString() const
{
    return m_streetSearchString;
}

QString NavigationModule::numberSearchString() const
{
    return m_numberSearchString;
}

QString NavigationModule::addressSearchString() const
{
    return m_addressSearchString;
}

void NavigationModule::clearPOISearch()
{
    setPOISearchString(QString());
    //clear doesn't seem to suffice to clear the model/notify the views
    m_poiSearchModel->setQuery(QSqlQuery());
    m_poiSearchModel->clear();
}

void NavigationModule::clearStreetSearch()
{
    setStreetSearchString(QString());
    //clear doesn't seem to suffice to clear the model/notify the views
    m_streetSearchModel->setQuery(QSqlQuery());
    m_streetSearchModel->clear();
}

void NavigationModule::clearCitySearch()
{
    setCitySearchString(QString());
    //clear doesn't seem to suffice to clear the model/notify the views
    m_citySearchModel->setQuery(QSqlQuery());
    m_citySearchModel->clear();
}

void NavigationModule::clearNumberSearch()
{
    setNumberSearchString(QString());
    //clear doesn't seem to suffice to clear the model/notify the views
    m_numberSearchModel->setQuery(QSqlQuery());
    m_numberSearchModel->clear();
}

void NavigationModule::handleLaunchRequest(const QJsonValue &dat)
{
    const QString action = dat.toObject().value(QStringLiteral("action")).toString();

    if (action == QStringLiteral("poisearch")) {
        const QString search_term = dat.toObject().value(QStringLiteral("search_term")).toString();
        m_workflow->setSheetMenuExtended(false);
        m_workflow->enterNavigationGridPage();
        m_workflow->enterSearchDestinationPage();
        m_workflow->backToSearchTypeSubpage();
        m_workflow->leaveSearchTypeSubpage(1);
        setPOISearchString(search_term);
        searchPOI(search_term);
    }else if(action == QStringLiteral("addresssearch")){
        const QJsonObject values = dat.toObject().value(QStringLiteral("location")).toObject();
        m_addressSearchString = values.value(QStringLiteral("number")).toString() + QStringLiteral(" ") +
                values.value(QStringLiteral("street")).toString() + QStringLiteral(" ") +
                values.value(QStringLiteral("city")).toString() + QStringLiteral(" ") +
                values.value(QStringLiteral("province")).toString();

        m_workflow->setSheetMenuExtended(false);
        m_workflow->enterNavigationGridPage();
        m_workflow->enterSearchDestinationPage();
        m_workflow->backToSearchTypeSubpage();
        m_workflow->enterSearchAddressSubpage();

        search(values.value(QStringLiteral("country")).toString(),
               values.value(QStringLiteral("city")).toString(),
               values.value(QStringLiteral("street")).toString(),
               values.value(QStringLiteral("number")).toString());
    } else {
        qCritical("%s: Unhandled action '%s'", Q_FUNC_INFO, qPrintable(action));
    }
}
