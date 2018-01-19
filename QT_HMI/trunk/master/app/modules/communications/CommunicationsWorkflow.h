#ifndef COMMUNICATIONSWORKFLOW_H
#define COMMUNICATIONSWORKFLOW_H

#include "util/Util.h"

#include <QObject>

class CommunicationsWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(CommunicationsWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(CommunicationsWorkflow::Overlay currentOverlay READ currentOverlay WRITE setCurrentOverlay NOTIFY currentOverlayChanged)
    Q_PROPERTY(bool sheetMenuExtended READ sheetMenuExtended WRITE setSheetMenuExtended NOTIFY sheetMenuExtendedChanged)
    Q_PROPERTY(QString enteredNumber READ enteredNumber WRITE setEnteredNumber NOTIFY enteredNumberChanged)
    Q_ENUMS(Page)
    Q_ENUMS(Overlay)

public:
    enum Page {
        GridPage,
        DialPadPage,
        ContactSearchPage
    };

    enum Overlay {
        NoOverlay,
        CallOverlay
    };

    explicit CommunicationsWorkflow(QObject *parent = 0);

    PROPERTY_WITH_NOTIFY(CommunicationsWorkflow::Page, currentPage, setCurrentPage, currentPageChanged)
    PROPERTY_WITH_NOTIFY(CommunicationsWorkflow::Overlay, currentOverlay, setCurrentOverlay, currentOverlayChanged)
    PROPERTY_WITH_NOTIFY(bool, sheetMenuExtended, setSheetMenuExtended, sheetMenuExtendedChanged)
    PROPERTY_WITH_NOTIFY(QString, enteredNumber, setEnteredNumber, enteredNumberChanged)

public Q_SLOTS:
    void enterGridPage();
    void enterDialPadPage();
    void enterContactSearchPage();
    void enterCallPage();
    void leaveCallPage();

Q_SIGNALS:
    void onEnterGridPage();
    void onEnterTextMessagesPage();
    void onEnterEmailMessagesPage();
    void onEnterAddressBookPage();
    void onEnterDialPadPage();
    void onEnterContactSearchPage();
    void onEnterCallPage();
    void onLeaveCallPage();

private:
    void initStateMachine();
};

#endif
