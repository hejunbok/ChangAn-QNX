#ifndef SETTINGSWORKFLOW_H
#define SETTINGSWORKFLOW_H

#include <QObject>

#include "util/Util.h"

class QState;

class SettingsWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SettingsWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(bool fluidLevelsDialogVisible READ fluidLevelsDialogVisible WRITE setFluidLevelsDialogVisible NOTIFY fluidLevelsDialogVisibleChanged)
    Q_PROPERTY(bool tractionSystemDialogVisible READ tractionSystemDialogVisible WRITE setTractionSystemDialogVisible NOTIFY tractionSystemDialogVisibleChanged)
    Q_PROPERTY(bool brakingSystemDialogVisible READ brakingSystemDialogVisible WRITE setBrakingSystemDialogVisible NOTIFY brakingSystemDialogVisibleChanged)
    Q_PROPERTY(bool powertrainSystemDialogVisible READ powertrainSystemDialogVisible WRITE setPowertrainSystemDialogVisible NOTIFY powertrainSystemDialogVisibleChanged)
    Q_PROPERTY(bool electricalSystemDialogVisible READ electricalSystemDialogVisible WRITE setElectricalSystemDialogVisible NOTIFY electricalSystemDialogVisibleChanged)
    Q_PROPERTY(bool vehicleStatisticsDialogVisible READ vehicleStatisticsDialogVisible WRITE setVehicleStatisticsDialogVisible NOTIFY vehicleStatisticsDialogVisibleChanged)

    Q_ENUMS(Page)

public:
    enum Page {
        /// Overview page
        SettingsGridPage,

        VehicleInfoPage,
        HeadlightsAmbientLightControlPage,
        SeatControlPage,
        AudioControlPage,
        DoorControlPage,
        PersonalizationPage,
    };

    explicit SettingsWorkflow(QObject* parent = 0);

    PROPERTY_WITH_NOTIFY(SettingsWorkflow::Page, currentPage, setCurrentPage, currentPageChanged)

    // virtual mechanic
    PROPERTY_WITH_NOTIFY(bool, fluidLevelsDialogVisible, setFluidLevelsDialogVisible, fluidLevelsDialogVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, tractionSystemDialogVisible, setTractionSystemDialogVisible, tractionSystemDialogVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, brakingSystemDialogVisible, setBrakingSystemDialogVisible, brakingSystemDialogVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, powertrainSystemDialogVisible, setPowertrainSystemDialogVisible, powertrainSystemDialogVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, electricalSystemDialogVisible, setElectricalSystemDialogVisible, electricalSystemDialogVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, vehicleStatisticsDialogVisible, setVehicleStatisticsDialogVisible, vehicleStatisticsDialogVisibleChanged)

public Q_SLOTS:
    void leave();

    // top-level pages
    void enterSettingsGridPage();
    void enterVehicleInfoPage();
    void enterHeadlightsAmbientLightControlPage();
    void enterSeatControlPage();
    void enterAudioControlPage();
    void enterDoorControlPage();
    void enterPersonalizationPage();

    // virtual mechanic pages
    void enterFluidLevelsDialog();
    void enterTractionSystemDialog();
    void enterBrakingSystemDialog();
    void enterPowertrainSystemDialog();
    void enterElectricalSystemDialog();
    void enterVehicleStatisticsDialog();

Q_SIGNALS:
    void onLeave();

    // top-level pages
    void onEnterSettingsGridPage();
    void onEnterVehicleInfoPage();
    void onEnterHeadlightsAmbientLightControlPage();
    void onEnterSeatControlPage();
    void onEnterAudioControlPage();
    void onEnterDoorControlPage();
    void onEnterPersonalizationPage();


    // virtual mechanic pages
    void onEnterFluidLevelsDialog();
    void onEnterTractionSystemDialog();
    void onEnterBrakingSystemDialog();
    void onEnterPowertrainSystemDialog();
    void onEnterElectricalSystemDialog();
    void onEnterVehicleStatisticsDialog();

private:
    void initStateMachine();
    void initVirtualMechanicWorkflow(QState*);
};

#endif
