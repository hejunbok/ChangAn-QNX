#include "MessagesModel.h"

#include "MessagesModel_p.h"

#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "util/DateTimeFormatting.h"

namespace QnxCar {

static const QString SQL_RECORD_LIMIT = QLatin1Literal(" LIMIT 20000");

static const QString SQL_SELECT_MESSAGES_TPL = QLatin1Literal(
    "SELECT "
    "message_id, "
    "handle, "
    "account_id, "
    "type, "
    "sender_last_name, "
    "sender_first_name, "
    "sender_email, "
    "sender_number, "
    "subject, "
    "datetime "
    "FROM messages_view "
);

static const QString SQL_ORDER = QLatin1Literal(" ORDER BY datetime DESC");

static const QString SQL_SELECT_MESSAGES_BY_TYPE = SQL_SELECT_MESSAGES_TPL + QLatin1Literal(
    " WHERE type = :messageType") +
    SQL_ORDER + SQL_RECORD_LIMIT;


static const QString SQL_SELECT_MESSAGES = SQL_SELECT_MESSAGES_TPL + SQL_ORDER + SQL_RECORD_LIMIT;

static const QString SQL_SELECT_PLAIN_TEXT = QLatin1Literal("SELECT "
    "body_plain_text "
    "FROM full_messages_view "
    "WHERE handle = :messageHandle AND account_id = :accountId"
);


static const QString SQL_SELECT_HTML = QLatin1Literal("SELECT "
    "body_html "
    "FROM full_messages_view "
    "WHERE handle = :messageHandle AND account_id = :accountId"
);


static MessagesModel::MessageType typeFromString(const QString &str)
{
    if (str == QStringLiteral("EMAIL"))
        return MessagesModel::EMail;
    else if (str == QStringLiteral("SMS_GSM"))
        return MessagesModel::SMS;

    qWarning() << "Unexpected message type" << str;
    return MessagesModel::SMS;
}

static QString typeToString(int type)
{
    switch (type) {
    case MessagesModel::EMail:
        return QStringLiteral("EMAIL");
    case MessagesModel::SMS:
        return QStringLiteral("SMS_GSM");
    }

    return QString();
}

QList<QByteArray> MessagesModel::columnNames() const
{
    return QList<QByteArray>()
        << "message_id"
        << "message_handle"
        << "account_id"
        << "type"
        << "sender_last_name"
        << "sender_first_name"
        << "sender_email"
        << "sender_number"
        << "subject"
        << "datetime";
}

MessagesModel::MessagesModel(QObject* parent)
    : SqlQueryModel(new Private, parent)
{
}

void MessagesModel::runQuery()
{
    reopenDatabase();
    if (D_PTR->messageType != -1) {
        const QString typeStr = typeToString(D_PTR->messageType);
        QSqlQuery query(database());
        query.prepare(SQL_SELECT_MESSAGES_BY_TYPE);
        query.bindValue(QStringLiteral(":messageType"), typeStr);
        query.exec();
        setQuery(query);
    } else {
        setQuery(SQL_SELECT_MESSAGES, database());
    }
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = SqlQueryModel::roleNames();
    roleNames.insert(DateTimeAsStringRole, "datetime_string");
    roleNames.insert(SenderLabelRole, "sender_label");
    return roleNames;
}

QVariant MessagesModel::data(const QModelIndex& item, int role) const
{
    const QVariant v = SqlQueryModel::data(item, role);

    switch (role) {
    case TypeRole:
        return typeFromString(v.toString());
    case DateTimeAsStringRole:
        return QnxCar::formatDateTime(data(item, DateTimeRole).toDateTime());
    case SenderLabelRole:
    {
        const QString firstName = data(item, SenderFirstNameRole).toString();
        const QString lastName = data(item, SenderLastNameRole).toString();
        const QString name = tr("%1 %2", "first name, surname").arg(firstName, lastName).trimmed();
        if (!name.isEmpty())
            return name;
        const QString number = data(item, SenderNumberRole).toString();
        if (!number.isEmpty())
            return number;
        const QString email = data(item, SenderEmailRole).toString();
        return email;
    }
    default:
        break;
    }

    return v;
}

void MessagesModel::setFilterMessageType(int type)
{
    D_PTR->messageType = type;
    update();
}

void MessagesModel::clearFilters()
{
    D_PTR->messageType = -1;
    update();
}

void MessagesModel::clear()
{
    //clear doesn't seem to suffice to clear the model/notify the views
    setQuery(QSqlQuery());
    SqlQueryModel::clear();
}

QString MessagesModel::messageBody(int accountId, const QString &messageHandle, TextFormat format) const
{
    QSqlQuery query(database());
    query.prepare(format == PlainText ? SQL_SELECT_PLAIN_TEXT : SQL_SELECT_HTML);
    query.bindValue(QStringLiteral(":accountId"), accountId);
    query.bindValue(QStringLiteral(":messageHandle"), messageHandle);
    if (!query.exec()) {
        qCritical("%s: %s", Q_FUNC_INFO, qPrintable(query.lastError().text()));
        return QString();
    }

    return query.next() ? query.value(0).toString() : QString();
}

}
