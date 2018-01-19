#ifndef QTQNXCAR2_MESSAGESMODEL_H
#define QTQNXCAR2_MESSAGESMODEL_H

#include "qtqnxcar2_export.h"

#include <qnxcar/SqlQueryModel.h>

namespace QnxCar {

/**
 * Model for accessing e-mail and text messages via Bluetooth.
 *
 * This model lists the messages available in the messages database (messages.db)
 *
 * To contain data, a bluetooth device supporting the messages profile must be connected.
 * The model will be empty if no device is connected.
 */
class QTQNXCAR2_EXPORT MessagesModel : public SqlQueryModel
{
    Q_OBJECT

    Q_ENUMS(MessageType)
    Q_ENUMS(TextFormat)

public:
    /// Item roles
    enum Role {
        /// Message identifier, role name @c "message_id"
        MessageIdRole = Qt::UserRole + 1,
        /// Message handle, role name @c "message_handle"
        MessageHandleRole,
        /// Account identifier, role name @c "account_id"
        AccountIdRole,
        /// Message type of value #Type, role name @c "type"
        TypeRole,
        /// Sender's last name, role name @c "sender_last_name"
        SenderLastNameRole,
        /// Sender's first name, role name @c "sender_first_name"
        SenderFirstNameRole,
        /// Sender's email address, role name @c "sender_email"
        SenderEmailRole,
        /// Sender's phone number, role name @c "sender_number"
        SenderNumberRole,
        /// Message subject, role name @c "subject"
        SubjectRole,
        /// Message time stamp, role name @c "datetime"
        DateTimeRole,
        /**
         * Sender as a single string, role name @c "sender_label"
         *
         * Best effort to get sender description (name if available, otherwise number or e-mail)
         */
        SenderLabelRole,
        /// Message time stamp as string (workaround for QTBUG-35693), role name @c "datetime_string"
        DateTimeAsStringRole
    };

    /// Message filter types
    enum MessageType {
        /// Messages that are emails
        EMail,
        /// Messsages that are SMS
        SMS
    };

    /// Text format to use when retrieving the message body
    enum TextFormat {
        PlainText,
        Html
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit MessagesModel(QObject *parent = 0);

    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * Reduce model contents to messages of a given @p type
     *
     * @param type a #MessageType
     * @sa clearFilters()
     */
    Q_INVOKABLE void setFilterMessageType(int type);

    /**
     * Clears filtering set by setFilterMessageType()
     */
    Q_INVOKABLE void clearFilters();

    void clear() Q_DECL_OVERRIDE;

    /**
     * Returns a message body of a message from the database.
     * The bodies are not fetched by default and must be requested separately via PPS.
     * Depending on the phone, the body will provided as HTML or plain text.
     * Thus you must
     */
    Q_INVOKABLE QString messageBody(int accountId, const QString &messageHandle, TextFormat format) const;

protected:
    void runQuery() Q_DECL_OVERRIDE;
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;

private:
    class Private;
};

}

#endif
