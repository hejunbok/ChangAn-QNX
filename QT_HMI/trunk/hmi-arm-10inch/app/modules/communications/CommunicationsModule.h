#ifndef COMMUNICATIONSMODULE_H
#define COMMUNICATIONSMODULE_H

#include "AbstractModule.h"
#include "services/BluetoothServiceNotification.h"
#include "services/BluetoothServiceStatus.h"
#include "services/Phone.h"

#include <QObject>
#include <QSortFilterProxyModel>

class CommunicationsWorkflow;


namespace QnxCar {
  class AddressbookModel;
  class AddressbookSearchModel;
  class Bluetooth;
  class BluetoothServiceStatus;
  class MessagesModel;
  class MessagesFilterModel;
}

namespace QPps {
    class Object;
}

class CommunicationsModule : public QObject, public AbstractModule
{
    Q_OBJECT
    Q_PROPERTY(CommunicationsWorkflow* workflow READ workflow CONSTANT)
    Q_PROPERTY(QnxCar::BluetoothServiceStatus* addressbookStatus READ addressbookStatus CONSTANT)
    Q_PROPERTY(QnxCar::BluetoothServiceStatus* messagesStatus READ messagesStatus CONSTANT)
    Q_PROPERTY(QnxCar::AddressbookModel* addressbookModel READ addressbookModel CONSTANT)
    Q_PROPERTY(QSortFilterProxyModel* sortedAddressbookModel READ sortedAddressbookModel CONSTANT)
    Q_PROPERTY(QnxCar::AddressbookSearchModel* addressbookSearchModel READ addressbookSearchModel CONSTANT)
    Q_PROPERTY(QSortFilterProxyModel* sortedAddressbookSearchModel READ sortedAddressbookSearchModel CONSTANT)
    Q_PROPERTY(QnxCar::MessagesModel* messagesModel READ messagesModel CONSTANT)
    Q_PROPERTY(QnxCar::MessagesFilterModel* messagesFilterModel READ messagesFilterModel CONSTANT)
    Q_PROPERTY(QString messageBodyPlainText READ messageBodyPlainText NOTIFY messageBodyPlainTextChanged)
    Q_PROPERTY(QString messageBodyHtml READ messageBodyHtml NOTIFY messageBodyHtmlChanged)

public:
    explicit CommunicationsModule(QObject *parent = 0);

    virtual void moduleReset() Q_DECL_OVERRIDE;
    virtual QString navigatorTabName() const Q_DECL_OVERRIDE;

    QnxCar::BluetoothServiceStatus *addressbookStatus() const;
    QnxCar::AddressbookModel *addressbookModel() const;
    QSortFilterProxyModel *sortedAddressbookModel() const;
    QnxCar::AddressbookSearchModel *addressbookSearchModel() const;
    QSortFilterProxyModel *sortedAddressbookSearchModel() const;
    QnxCar::BluetoothServiceStatus *messagesStatus() const;
    QnxCar::MessagesModel *messagesModel() const;
    QnxCar::MessagesFilterModel *messagesFilterModel() const;

    QString messageBodyPlainText() const;
    QString messageBodyHtml() const;

    void handleLaunchRequest(const QJsonValue &dat);

    int carplayState() const;
    int mirrorlinkState() const;

Q_SIGNALS:
    void messageBodyPlainTextChanged(const QString &);
    void messageBodyHtmlChanged(const QString &);
    void carplayStateChanged();

public Q_SLOTS:
    void requestMessageBody(int accountId, const QString &messageHandle);
    void carplayOperation();

    void mirrorLinkStart();
    void mirrorLinkStop();

private Q_SLOTS:
    void onHandsFreeProfileAvailableChanged();
    void onPhoneStateChanged(QnxCar::Phone::State state);
    void onAddressbookStatusChanged();
    void onMessagesStateChanged(QnxCar::BluetoothServiceStatus::State state);
    void onMessagesStatusChanged(QnxCar::BluetoothServiceStatus::Status status);
    void onMessagesNotification(int accountId, const QString &handle,
                                QnxCar::BluetoothServiceNotification::MessageType type,
                                QnxCar::BluetoothServiceNotification::Status status);

    void updateMessageBodies();
    /**
     * @brief onCallEvent generic call event handler
     * @param callId the call ID
     * @param service the phone service
     */
    void onCallEvent (const QString &callId, const QString &service);
    /**
     * @brief onOutgoingCallEvent specific handler for the outgoing call event,
     *        contains logic to grab the number from the HMI if no number is returned
     *        from HFP
     * @param callId the call ID
     * @param service the phone service
     */
    void onOutgoingCallEvent (const QString &callId, const QString &service);


private:
    CommunicationsWorkflow * workflow() const;
    CommunicationsWorkflow * const m_workflow;
    QnxCar::BluetoothServiceStatus *const m_addressbookStatus;
    QnxCar::BluetoothServiceStatus *const m_messagesStatus;
    QnxCar::BluetoothServiceNotification *const m_messagesNotification;
    QnxCar::Bluetooth *const m_bluetooth;
    QnxCar::Phone *const m_phone;
    QnxCar::AddressbookModel *m_addressbookModel;
    QnxCar::AddressbookSearchModel *m_addressbookSearchModel;
    QnxCar::MessagesModel *m_messagesModel;
    QnxCar::MessagesFilterModel *m_messagesFilterModel;
    QPps::Object *m_ppsMessagesControl;
    QSortFilterProxyModel *m_sortedAddressbookModel;
    QSortFilterProxyModel *m_sortedAddressbookSearchModel;

    struct MessageBody {
        MessageBody();

        QString plainText;
        QString html;
        int accountId;
        QString messageHandle;
        bool pendingRequest;
    };

    MessageBody m_messageBody;

    int m_carplayState;
    int m_mirrorlinkState;
};

#endif
