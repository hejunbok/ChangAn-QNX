#ifndef NAVIGATIONMODULE_H
#define NAVIGATIONMODULE_H

#include "AbstractModule.h"

#include <QObject>

class NavigationWorkflow;

namespace QnxCar {
    class LocationCategoryModel;
    class LocationModel;
    class LocationObject;
    class Navigation;
    class NavigationHistoryModel;
    class FavouriteModel;
    class NavigationSettingsModel;
}

class NavigationModule : public QObject, public AbstractModule
{
    Q_OBJECT

    Q_PROPERTY(NavigationWorkflow* workflow READ workflow CONSTANT)
    Q_PROPERTY(QnxCar::LocationCategoryModel *locationCategoryModel READ locationCategoryModel CONSTANT)
    Q_PROPERTY(QnxCar::LocationModel* locationModel READ locationModel CONSTANT)
    Q_PROPERTY(QString poiSearchString READ poiSearchString WRITE setPOISearchString NOTIFY poiSearchStringChanged)
    Q_PROPERTY(QString citySearchString READ citySearchString WRITE setCitySearchString NOTIFY citySearchStringChanged)
    Q_PROPERTY(QString streetSearchString READ streetSearchString WRITE setStreetSearchString NOTIFY streetSearchStringChanged)
    Q_PROPERTY(QString numberSearchString READ numberSearchString WRITE setNumberSearchString NOTIFY numberSearchStringChanged)
    Q_PROPERTY(QString addressSearchString READ addressSearchString WRITE setAddressSearchString NOTIFY addressSearchStringChanged)
    Q_PROPERTY(QnxCar::LocationModel* poiSearchModel READ poiSearchModel CONSTANT)
    Q_PROPERTY(QnxCar::LocationModel* citySearchModel READ citySearchModel CONSTANT)
    Q_PROPERTY(QnxCar::LocationModel* streetSearchModel READ streetSearchModel CONSTANT)
    Q_PROPERTY(QnxCar::LocationModel* numberSearchModel READ numberSearchModel CONSTANT)
    Q_PROPERTY(QnxCar::LocationModel* addressSearchModel READ addressSearchModel CONSTANT)
    Q_PROPERTY(QnxCar::Navigation* navigation READ navigation CONSTANT)
    Q_PROPERTY(QnxCar::FavouriteModel* favouriteModel READ favouriteModel CONSTANT)
    Q_PROPERTY(QnxCar::NavigationHistoryModel* historyModel READ historyModel CONSTANT)
    Q_PROPERTY(QnxCar::LocationObject* currentDestination READ currentDestination CONSTANT)
    Q_PROPERTY(QnxCar::NavigationSettingsModel* settingModel READ settingModel CONSTANT)

public:
    explicit NavigationModule(QObject *parent = 0);

    QnxCar::LocationModel *locationModel() const;

    QnxCar::LocationModel *poiSearchModel() const;

    QnxCar::LocationModel *citySearchModel() const;

    QnxCar::LocationModel *streetSearchModel() const;

    QnxCar::LocationModel *numberSearchModel() const;

    QnxCar::LocationModel *addressSearchModel() const;

    QnxCar::Navigation *navigation() const;

    QnxCar::NavigationSettingsModel *settingModel() const;

    virtual void moduleReset() Q_DECL_OVERRIDE;
    virtual void moduleSelected() Q_DECL_OVERRIDE;
    virtual void moduleDeselected() Q_DECL_OVERRIDE;
    virtual QString navigatorTabName() const Q_DECL_OVERRIDE;

    QnxCar::LocationCategoryModel *locationCategoryModel() const;

    QnxCar::FavouriteModel *favouriteModel() const;

    QnxCar::NavigationHistoryModel *historyModel() const;

    QnxCar::LocationObject *currentDestination() const;

    Q_INVOKABLE void navigateToLocation(int locationRow);
    Q_INVOKABLE void navigateToPOI(int poiRow);
    Q_INVOKABLE void navigateToFavourite(int favouriteRow);
    Q_INVOKABLE void navigateToHistory(int historyRow);
    Q_INVOKABLE void navigateToSearchResult(int locationRow);
    Q_INVOKABLE void navigateToAddressResult(int locationRow);

    Q_INVOKABLE void addFavourite(int historyRow);
    Q_INVOKABLE void deleteFavourite(int favouriteRow);

    Q_INVOKABLE void searchPOI(const QString &name);
    Q_INVOKABLE void search(const QString &country, const QString &city, const QString &street, const QString &number);

    QString poiSearchString() const;
    void setPOISearchString(const QString &poiSearchString);

    QString citySearchString() const;
    void setCitySearchString(const QString &citySearchString);

    QString streetSearchString() const;
    void setStreetSearchString(const QString &streetSearchString);

    QString numberSearchString() const;
    void setNumberSearchString(const QString &numberSearchString);

    QString addressSearchString() const;
    void setAddressSearchString(const QString &addressSearchString);

    void handleLaunchRequest(const QJsonValue &dat);

Q_SIGNALS:
    void poiSearchStringChanged(const QString&);
    void citySearchStringChanged(const QString&);
    void streetSearchStringChanged(const QString&);
    void numberSearchStringChanged(const QString&);
    void addressSearchStringChanged(const QString&);

public Q_SLOTS:
    void clearPOISearch();
    void clearStreetSearch();
    void clearNumberSearch();
    void clearCitySearch();

private:
    void navigateTo(const QModelIndex &index, int locationRole);
    NavigationWorkflow *workflow() const;

    NavigationWorkflow *m_workflow;
    QnxCar::LocationCategoryModel *m_locationCategoryModel;
    QnxCar::LocationModel *m_locationModel;
    QnxCar::LocationModel *m_poiSearchModel;
    QnxCar::LocationModel *m_citySearchModel;
    QnxCar::LocationModel *m_streetSearchModel;
    QnxCar::LocationModel *m_numberSearchModel;
    QnxCar::LocationModel *m_addressSearchModel;
    QnxCar::Navigation *m_navigation;
    QnxCar::FavouriteModel *m_favouriteModel;
    QnxCar::NavigationHistoryModel *m_historyModel;
    QnxCar::LocationObject *m_currentDestination;
    QnxCar::NavigationSettingsModel *m_settingModel;

    QString m_poiSearchString;
    QString m_citySearchString;
    QString m_streetSearchString;
    QString m_numberSearchString;
    QString m_addressSearchString;
};

#endif
