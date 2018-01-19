#ifndef __SMARTDEVICE_WORKFLOW_H
#define __SMARTDEVICE_WORKFLOW_H

#include <QObject>

#include "util/Util.h"

class QState;

class SmartDeviceWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SmartDeviceWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(bool watchDialogVisible READ watchDialogVisible WRITE setWatchDialogVisible NOTIFY watchDialogVisibleChanged)
    Q_PROPERTY(bool glassDialogVisible READ glassDialogVisible WRITE setGlassDialogVisible NOTIFY glassDialogVisibleChanged)
    Q_ENUMS(Page)

public:
    enum Page {
        PersonalSettingPage,
        /// Overview page
        WatchControlPage,
        GlassControlPage
    };

    explicit SmartDeviceWorkflow(QObject* parent = 0);

    PROPERTY_WITH_NOTIFY(SmartDeviceWorkflow::Page, currentPage, setCurrentPage, currentPageChanged)

    // virtual mechanic
    PROPERTY_WITH_NOTIFY(bool, watchDialogVisible, setWatchDialogVisible, watchDialogVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, glassDialogVisible, setGlassDialogVisible, glassDialogVisibleChanged)

public Q_SLOTS:
    void leave();

    // top-level pages
    void enterPersonalSettingPage();

    // virtual mechanic pages
    void enterWatchDialog();
    void enterGlassDialog();

Q_SIGNALS:
    void onLeave();

    // top-level pages
    void onEnterPersonalSettingPage();

    // virtual mechanic pages
    void onEnterWatchDialog();
    void onEnterGlassDialog();

private:
    void initStateMachine();
    void initSmartDeviceWorkflow(QState*);
};

#endif
