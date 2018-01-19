#include "CommunicationsModule.h"

#include "CommunicationsWorkflow.h"

#include "qnxcar/AddressbookModel.h"
#include "qnxcar/AddressbookSearchModel.h"
#include "qnxcar/DatabaseManager.h"
#include "qnxcar/MessagesModel.h"
#include "qnxcar/MessagesFilterModel.h"
#include "qnxcar/CallerIDObject.h"
#include "services/Bluetooth.h"

#include <qpps/object.h>
#include <qpps/changeset.h>

#include <qqml.h>
#include <QSqlError>
#include <QJsonObject>

using namespace QnxCar;

CommunicationsModule::MessageBody::MessageBody()
    : accountId(0)
    , pendingRequest(false)
{
}

CommunicationsModule::CommunicationsModule(QObject *parent)
    : QObject(parent)
    , m_workflow(new CommunicationsWorkflow(this))
    , m_addressbookStatus(new BluetoothServiceStatus(BluetoothServiceStatus::Addressbook, this))
    , m_messagesStatus(new BluetoothServiceStatus(BluetoothServiceStatus::Messages, this))
    , m_messagesNotification(new BluetoothServiceNotification(this))
    , m_bluetooth(new Bluetooth(this))
    , m_phone(new Phone(this))
    , m_ppsMessagesControl(new QPps::Object(QStringLiteral("/pps/services/bluetooth/messages/control"), QPps::Object::PublishMode, true, this))
    , m_carplayState(0)
    , m_mirrorlinkState(0)
{

    qmlRegisterType<QSortFilterProxyModel>();
    qmlRegisterUncreatableType<CommunicationsWorkflow>(
        "com.qnx.car.ui", 2, 0, "CommunicationsWorkflow",
        QStringLiteral("Access to enums"));

    qmlRegisterUncreatableType<BluetoothServiceStatus>("com.qnx.car.ui", 2, 0,
        "BluetoothServiceStatus", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<BluetoothServiceNotification>("com.qnx.car.ui", 2, 0,
        "BluetoothServiceNotification", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<AddressbookModel>("com.qnx.car.ui", 2, 0,
        "AddressbookModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<AddressbookSearchModel>("com.qnx.car.ui", 2, 0,
        "AddressbookSearchModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<MessagesModel>("com.qnx.car.ui", 2, 0,
        "MessagesModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<MessagesFilterModel>("com.qnx.car.ui", 2, 0,
        "MessagesFilterModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<CallerIDObject>("com.qnx.car.ui", 2, 0,
        "CallerIDObject", QStringLiteral("Access to object"));

    QSqlDatabase addressDb = DatabaseManager::database(QStringLiteral("phonebook.db"));

    m_addressbookModel = new AddressbookModel(this);
    m_addressbookModel->setDatabase(addressDb);

    m_sortedAddressbookModel = new QSortFilterProxyModel(this);
    m_sortedAddressbookModel->setDynamicSortFilter(true);
    m_sortedAddressbookModel->setSourceModel(m_addressbookModel);
    m_sortedAddressbookModel->setSortRole(QnxCar::AddressbookModel::DisplayName);
    m_sortedAddressbookModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortedAddressbookModel->sort(0);

    m_addressbookSearchModel = new AddressbookSearchModel(this);
    m_addressbookSearchModel->setDatabase(addressDb);

    m_sortedAddressbookSearchModel = new QSortFilterProxyModel(this);
    m_sortedAddressbookSearchModel->setDynamicSortFilter(true);
    m_sortedAddressbookSearchModel->setSourceModel(m_addressbookSearchModel);
    m_sortedAddressbookSearchModel->setSortRole(QnxCar::AddressbookSearchModel::DisplayName);
    m_sortedAddressbookSearchModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortedAddressbookSearchModel->sort(0);

    QSqlDatabase messageDb = DatabaseManager::database(QStringLiteral("messages.db"));

    m_messagesModel = new MessagesModel(this);
    m_messagesModel->setDatabase(messageDb);

    m_messagesFilterModel = new MessagesFilterModel(this);
    m_messagesFilterModel->setDynamicSortFilter(true);
    m_messagesFilterModel->setSourceModel(m_messagesModel);

    if (!m_ppsMessagesControl->isValid()) {
        qWarning("%s: Could not open '%s': %s", Q_FUNC_INFO, qPrintable(m_ppsMessagesControl->path()), qPrintable(m_ppsMessagesControl->errorString()));
    }
    connect(m_bluetooth, &Bluetooth::handsFreeProfileAvailableChanged,
            this, &CommunicationsModule::onHandsFreeProfileAvailableChanged);
    connect(m_phone, &Phone::stateChanged,
            this, &CommunicationsModule::onPhoneStateChanged);
    connect(m_phone, &Phone::incomingCall,
            this, &CommunicationsModule::onCallEvent);
    connect(m_phone, &Phone::outgoingCall,
            this, &CommunicationsModule::onOutgoingCallEvent);
    connect(m_phone, &Phone::callActivated,
            this, &CommunicationsModule::onCallEvent);
    connect(m_addressbookStatus, &BluetoothServiceStatus::statusChanged,
            this, &CommunicationsModule::onAddressbookStatusChanged);
    connect(m_addressbookStatus, &BluetoothServiceStatus::stateChanged,
            this, &CommunicationsModule::onAddressbookStatusChanged);
    connect(m_messagesStatus, &BluetoothServiceStatus::statusChanged,
            this, &CommunicationsModule::onMessagesStatusChanged);
    connect(m_messagesStatus, &BluetoothServiceStatus::stateChanged,
            this, &CommunicationsModule::onMessagesStateChanged);
    connect(m_messagesNotification, &BluetoothServiceNotification::notification,
            this, &CommunicationsModule::onMessagesNotification);
}

void CommunicationsModule::moduleReset()
{
    m_workflow->setSheetMenuExtended(false);
    m_workflow->enterGridPage();
}

void CommunicationsModule::requestMessageBody(int accountId, const QString &messageHandle)
{
    m_messageBody.pendingRequest = true;
    m_messageBody.accountId = accountId;
    m_messageBody.messageHandle = messageHandle;
    m_messageBody.plainText.clear();
    m_messageBody.html.clear();
    emit messageBodyPlainTextChanged(m_messageBody.plainText);
    emit messageBodyHtmlChanged(m_messageBody.html);

#ifndef CAR2_HOST_BUILD
    QPps::Changeset changes;
    changes.assignments.insert(QStringLiteral("command"), QStringLiteral("GET_MESSAGE"));
    changes.assignments.insert(QStringLiteral("account_id"), accountId);
    changes.assignments.insert(QStringLiteral("message_folder"), QString());
    changes.assignments.insert(QStringLiteral("message_handle"), messageHandle);
    if (!m_ppsMessagesControl->setAttributes(changes)) {
        qWarning("%s: Could not write to PPS: %s", Q_FUNC_INFO, qPrintable(m_ppsMessagesControl->errorString()));
    }
#else
    //On the desktop, just fetch from the db
    QMetaObject::invokeMethod(this, "updateMessageBodies", Qt::QueuedConnection);
#endif
}

void CommunicationsModule::carplayOperation()
{
    if (m_carplayState == 0)
    {
        m_carplayState = 1;
        system("/opt/airplay.sh start &");
    }
    else
    {
        m_carplayState = 0;
        system("/opt/airplay.sh stop &");
    }

    emit carplayStateChanged();
}

void CommunicationsModule::mirrorLinkStart()
{
    m_mirrorlinkState = 1;
    system("/opt/runml.sh start &");
}

void CommunicationsModule::mirrorLinkStop()
{
    m_mirrorlinkState = 0;
    system("/opt/runml.sh stop &");
}

void CommunicationsModule::onHandsFreeProfileAvailableChanged()
{
    const bool available = m_bluetooth->handsFreeProfileAvailable();
    // Return from dial pad to main view, once we lose the active
    // Bluetooth connection:
    if (!available) {
        m_workflow->enterGridPage();
    }
}

void CommunicationsModule::onPhoneStateChanged(Phone::State state)
{
    switch(state) {
    case Phone::CallIncomingState:
    case Phone::CallOutgoingDialingState:
        m_workflow->enterCallPage();
        break;

    case Phone::InitializedState:
        m_workflow->leaveCallPage();
        break;
    case Phone::ConnectedIdleState:
        m_workflow->leaveCallPage();
        // We clear the number here to avoid incorret contact data being shown
        // in case where we do a contact lookup based on this number
        m_workflow->setEnteredNumber(QLatin1String(""));
        break;

    // FIXME: Don't know yet how to handle all the other possible states.
    default:
        break;
    }
}

void CommunicationsModule::onAddressbookStatusChanged()
{
    const BluetoothServiceStatus::State state = m_addressbookStatus->state();
    const BluetoothServiceStatus::Status status = m_addressbookStatus->status();
    if (state == BluetoothServiceStatus::Disconnected) {
        m_addressbookModel->clear();
        m_addressbookSearchModel->clear();
    } else if (status == BluetoothServiceStatus::Complete && state == BluetoothServiceStatus::Connected) {
        m_addressbookModel->update();
        m_addressbookSearchModel->update();
    }
}

//QML Text doesn't like window linebreaks in plain text, so remove it
static QString cleanBody(QString s)
{
    s.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    return s;
}

void CommunicationsModule::updateMessageBodies()
{
    if (!m_messageBody.pendingRequest)
        return;
    m_messageBody.pendingRequest = false;
    m_messageBody.plainText = cleanBody(m_messagesModel->messageBody(m_messageBody.accountId, m_messageBody.messageHandle, MessagesModel::PlainText));
    m_messageBody.html = m_messagesModel->messageBody(m_messageBody.accountId, m_messageBody.messageHandle, MessagesModel::Html).trimmed();
    emit messageBodyPlainTextChanged(m_messageBody.plainText);
    emit messageBodyHtmlChanged(m_messageBody.html);
}

void CommunicationsModule::onOutgoingCallEvent(const QString &callId, const QString &/*service*/)
{
//  since the callID is empty grab the entered number in the workflow
    if(callId.isEmpty()){
        m_addressbookModel->setCallerIDString(m_workflow->enteredNumber());
    }else {
        m_addressbookModel->setCallerIDString(callId);
    }
}
void CommunicationsModule::onCallEvent(const QString &callId, const QString &/*service*/)
{
        m_addressbookModel->setCallerIDString(callId);
}

void CommunicationsModule::onMessagesStateChanged(QnxCar::BluetoothServiceStatus::State state)
{
    if (state == BluetoothServiceStatus::Disconnected) {
        // Clear all messages
        m_messagesModel->clear();
    } else if (state == BluetoothServiceStatus::Connected) {
        // Load the message list
        m_messagesModel->update();
    }
}

void CommunicationsModule::onMessagesStatusChanged(BluetoothServiceStatus::Status status)
{
    if (m_messageBody.pendingRequest && status == BluetoothServiceStatus::Complete) {
        updateMessageBodies();
    }
}

void CommunicationsModule::onMessagesNotification(int accountId, const QString &handle,
                                                  QnxCar::BluetoothServiceNotification::MessageType type,
                                                  QnxCar::BluetoothServiceNotification::Status status)
{
    Q_UNUSED(accountId);
    Q_UNUSED(handle);
    Q_UNUSED(type);

    switch (status) {
    case BluetoothServiceNotification::NewMessage:
    case BluetoothServiceNotification::MessageDeleted:
    case BluetoothServiceNotification::MessageShift:
        m_messagesModel->update();
        break;
    default:
        break;
    }
}

CommunicationsWorkflow * CommunicationsModule::workflow() const
{
    return m_workflow;
}

BluetoothServiceStatus * CommunicationsModule::addressbookStatus() const
{
    return m_addressbookStatus;
}

AddressbookModel * CommunicationsModule::addressbookModel() const
{
    return m_addressbookModel;
}
QSortFilterProxyModel * CommunicationsModule::sortedAddressbookModel() const
{
    return m_sortedAddressbookModel;
}
AddressbookSearchModel * CommunicationsModule::addressbookSearchModel() const
{
    return m_addressbookSearchModel;
}
QSortFilterProxyModel * CommunicationsModule::sortedAddressbookSearchModel() const
{
    return m_sortedAddressbookSearchModel;
}
MessagesModel * CommunicationsModule::messagesModel() const
{
    return m_messagesModel;
}

MessagesFilterModel * CommunicationsModule::messagesFilterModel() const
{
    return m_messagesFilterModel;
}

BluetoothServiceStatus * CommunicationsModule::messagesStatus() const
{
    return m_messagesStatus;
}

QString CommunicationsModule::navigatorTabName() const
{
    return QStringLiteral("Communication");
}

QString CommunicationsModule::messageBodyPlainText() const
{
    return m_messageBody.plainText;
}

QString CommunicationsModule::messageBodyHtml() const
{
    return m_messageBody.html;
}

void CommunicationsModule::handleLaunchRequest(const QJsonValue &dat)
{
    const QString action = dat.toObject().value(QStringLiteral("action")).toString();

    if (action == QStringLiteral("contact-list")) {
        m_workflow->setSheetMenuExtended(false);
        m_workflow->enterGridPage();
        m_workflow->enterContactSearchPage();
        m_addressbookSearchModel->update();
    } else {
        qCritical("%s: Unhandled action '%s'", Q_FUNC_INFO, qPrintable(action));
    }
}

int CommunicationsModule::carplayState() const
{
    return m_carplayState;
}

int CommunicationsModule::mirrorlinkState() const
{
    return m_mirrorlinkState;
}
