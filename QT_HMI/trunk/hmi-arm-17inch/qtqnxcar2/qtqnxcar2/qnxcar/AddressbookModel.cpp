#include "AddressbookModel.h"
#include "qnxcar/CallerIDObject.h"
#include "qnxcar/CallerID_p.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QStringList>
#include <QRegularExpression>
#include <QDebug>

namespace QnxCar {

struct AddressbookModel::Private
{
    CallerIDObject *callerID;
};

QList<QByteArray> AddressbookModel::columnNames() const {
    return QList<QByteArray>()
    << "contact_id"
    << "last_name"
    << "first_name"
    << "formatted_name"
    << "birthday"
    << "anniversary"
    << "company"
    << "job_title"
    << "home_phone"
    << "home_phone_2"
    << "work_phone"
    << "work_phone_2"
    << "mobile_phone"
    << "pager_phone"
    << "fax_phone"
    << "other_phone"
    << "email_1"
    << "email_2"
    << "email_3"
    << "home_address_1"
    << "home_address_2"
    << "home_address_city"
    << "home_address_country"
    << "home_address_state_province"
    << "home_address_zip_postal"
    << "work_address_1"
    << "work_address_2"
    << "work_address_city"
    << "work_address_country"
    << "work_address_state_province"
    << "work_address_zip_postal"
    << "picture"
    << "pin"
    << "web_page"
    << "note";
}

static const QString SQL_RECORD_LIMIT = QLatin1Literal(" LIMIT 20000");

static const QString SQL_SELECT_CONTACTS_TPL = QLatin1Literal(
    "SELECT "
    "contact_id, "
    "last_name, "
    "first_name, "
    "formatted_name, "
    "birthday, "
    "anniversary, "
    "company, "
    "job_title, "
    "home_phone, "
    "home_phone_2, "
    "work_phone, "
    "work_phone_2, "
    "mobile_phone, "
    "pager_phone, "
    "fax_phone, "
    "other_phone, "
    "email_1, "
    "email_2, "
    "email_3, "
    "home_address_1, "
    "home_address_2, "
    "home_address_city, "
    "home_address_country, "
    "home_address_state_province, "
    "home_address_zip_postal, "
    "work_address_1, "
    "work_address_2, "
    "work_address_city, "
    "work_address_country, "
    "work_address_state_province, "
    "work_address_zip_postal, "
    "picture, "
    "pin, "
    "web_page, "
    "note "
    "FROM contacts_view"
);

static const QString SQL_SELECT_CONTACTS = SQL_SELECT_CONTACTS_TPL + SQL_RECORD_LIMIT;


static const QString SQL_SELECT_CALLERID = QLatin1Literal(
    "SELECT "
    "first_name, "
    "last_name, "
    "picture, "
    "home_phone, "
    "home_phone_2, "
    "work_phone, "
    "work_phone_2, "
    "mobile_phone, "
    "other_phone "
    "FROM contacts_view "
    "WHERE home_phone LIKE :pattern OR "
    "home_phone_2 LIKE :pattern OR "
    "work_phone LIKE :pattern OR "
    "work_phone_2 LIKE :pattern OR "
    "mobile_phone LIKE :pattern OR "
    "other_phone LIKE :pattern"
);

struct IsEmpty
{
    bool operator()(const QString &s) const
    {
        return s.isEmpty();
    }
};

static QStringList removeEmpty(QStringList l)
{
    l.erase(std::remove_if(l.begin(), l.end(), IsEmpty()), l.end());
    return l;
}

static QString formatAddress(const QString &street,
                             const QString &extension,
                             const QString &city,
                             const QString &province,
                             const QString &country,
                             const QString &zip)
{
    const QString lastLine = removeEmpty(QStringList() << city << province << country << zip).join(QStringLiteral(", "));
    const QStringList lines = removeEmpty(QStringList() << street << extension << lastLine);
    return lines.join(QStringLiteral("\n")).trimmed();
}
static QString formatName(const QString &firstName,
                          const QString &lastName,
                          const QString &formattedName,
                          const QString &company,
                          const QString &homePhone1,
                          const QString &mobilePhone,
                          const QString &homePhone2,
                          const QString &workPhone1,
                          const QString &workPhone2,
                          const QString &pagerPhone,
                          const QString &email1,
                          const QString &email2,
                          const QString &email3,
                          const QString &webPage)
{
    QString combinedName = firstName.trimmed() + " " + lastName.trimmed();
    const QStringList names = removeEmpty(QStringList()
                                          << combinedName.trimmed()
                                          << formattedName
                                          << company
                                          << homePhone1
                                          << mobilePhone
                                          << homePhone2
                                          << workPhone1
                                          << workPhone2
                                          << pagerPhone
                                          << email1
                                          << email2
                                          << email3
                                          << webPage);
    if(names.size() > 0){
        return names.at(0);
    }else{
        return "";
    }
}
AddressbookModel::AddressbookModel(QObject* parent)
    : SqlQueryModel(parent), d(new Private)
{
    d->callerID = new CallerIDObject();
}

AddressbookModel::~AddressbookModel()
{
    delete d;
}

void AddressbookModel::runQuery()
{
    reopenDatabase();
    setQuery(SQL_SELECT_CONTACTS, database());
    //TODO: test this with a large contact set around ~10000
    while (this->canFetchMore()){
        this->fetchMore();
    }
}

QHash< int, QByteArray > AddressbookModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = SqlQueryModel::roleNames();
    roleNames.insert(FormattedHomeAddress, "formatted_home_address");
    roleNames.insert(FormattedWorkAddress, "formatted_work_address");
    roleNames.insert(DisplayName, "display_name");
    return roleNames;
}

QVariant AddressbookModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case FormattedHomeAddress:
    {
        const QString street = index.data(HomeAddressStreetRole).toString();
        const QString ext = index.data(HomeAddressExtendedRole).toString();
        const QString city = index.data(HomeAddressCityRole).toString();
        const QString province = index.data(HomeAddressProvinceRole).toString();
        const QString country = index.data(HomeAddressCountryRole).toString();
        const QString zip = index.data(HomeAddressZipCodeRole).toString();
        return formatAddress(street, ext, city, province, country, zip);
    }

    case FormattedWorkAddress:
    {
        const QString street = index.data(WorkAddressStreetRole).toString();
        const QString ext = index.data(WorkAddressExtendedRole).toString();
        const QString city = index.data(WorkAddressCityRole).toString();
        const QString province = index.data(WorkAddressProvinceRole).toString();
        const QString country = index.data(WorkAddressCountryRole).toString();
        const QString zip = index.data(WorkAddressZipCodeRole).toString();
        return formatAddress(street, ext, city, province, country, zip);
    }
    case DisplayName:
    {
        const QString firstName = index.data(FirstNameRole).toString();
        const QString lastName = index.data(LastNameRole).toString();
        const QString formattedName = index.data(FormattedNameRole).toString();
        const QString company = index.data(CompanyRole).toString();
        const QString homePhone1 = index.data(HomePhone1Role).toString();
        const QString mobilePhone = index.data(MobilePhoneRole).toString();
        const QString homePhone2 = index.data(HomePhone2Role).toString();
        const QString workPhone1 = index.data(WorkPhone1Role).toString();
        const QString workPhone2 = index.data(WorkPhone2Role).toString();
        const QString pagerPhone = index.data(PagerPhoneRole).toString();
        const QString email1 = index.data(Email1Role).toString();
        const QString email2 = index.data(Email2Role).toString();
        const QString email3 = index.data(Email3Role).toString();
        const QString webPage = index.data(WebPageRole).toString();

        return formatName(firstName, lastName, formattedName, company, homePhone1, mobilePhone,
                                  homePhone2, workPhone1, workPhone2, pagerPhone, email1, email2, email3,
                                  webPage);
    }
    default:
        break;
    }

    return SqlQueryModel::data(index, role);
}

QString sanitizeNumber(const QString &number)
{
    const QRegularExpression re(QStringLiteral("\\D"));
    QString sanitzedCallerIDString(number);
    sanitzedCallerIDString.replace(re, QStringLiteral(""));
    return sanitzedCallerIDString;
}

struct Contact {
    QString name;
    QString picture;
    QVector<QString> numbers;
};

void AddressbookModel::setCallerIDString(const QString &callerIDString)
{
    CallerID callID;
    //  We set the callerID with the freshly inititalized object to ensure that
    //  there is no stale data leftover from any previous call
    d->callerID->setCallerID(callID);
    //  if the value passed in is an empty string we exit early
    //  since we will receive multiple events from the phone service
    //  and its not uncommon to receive 1 or 2 events with an empty callerIDString
    //  before we get the actual number
    if(callerIDString == ""){
        return;
    }

    callID.number = callerIDString;

    const QString sanitizedCallerID = sanitizeNumber(callerIDString);
    // NOTE: This assumes that the phone numbers we're checking against are significantly unique to 7 digits, starting from the right
    const int significantDigits = 7;
    const QString pattern =  "%" + sanitizedCallerID.right(1);
    QSqlQuery query(database());
    query.prepare(SQL_SELECT_CALLERID);
    query.bindValue(QStringLiteral(":pattern"), pattern);
    if (!query.exec()) {
        qWarning("%s: %s", Q_FUNC_INFO, qPrintable(query.lastError().text()));
        return;
    }

    // Attempt to find an exact match in the results
    while (query.next()) {
        for (int i = 3; i < 9; ++i){
            if (sanitizeNumber(query.value(i).toString()) == sanitizedCallerID) {
                callID.name = query.value(0).toString() + QStringLiteral(" ") + query.value(1).toString();
                callID.picture = query.value(2).toString();
                d->callerID->setCallerID(callID);
                return;
            }
        }
    }

    // If we haven't already found a match, begin partial matching on the results, but
    // only if the callId has enough significant digits.
    if (sanitizedCallerID.count() >= significantDigits) {
        query.seek(-1);

        // Build up a list of contact with the numbers that match the last digit
        QVector<Contact> contacts;
        while (query.next()) {
            Contact contact;
            contact.name = query.value(0).toString() + QStringLiteral(" ") + query.value(1).toString();
            contact.picture = query.value(2).toString();
            for (int i = 3; i < 9; ++i){
                const QString number = sanitizeNumber(query.value(i).toString());
                if (!number.isEmpty())
                    contact.numbers.append(number);
            }
            contacts.append(contact);
        }
        // If no exact match was found, we can drill down by walking backwards through
        // the string and pruning contacts that don't have a phone number with the same
        // digit in that position.
        for (int pos = 1; pos <= significantDigits; ++pos) {
            const QChar currentDigit = callerIDString.at(callerIDString.size() - pos);
            QVector<Contact>::Iterator cit = contacts.begin();
            // Go through contacts one by one
            while (cit != contacts.end()) {
                QVector<QString>::Iterator nit = cit->numbers.begin();
                // Go through numbers of contact one by one
                while (nit != cit->numbers.end()) {
                    if (nit->at(nit->size() - pos) != currentDigit) {
                        // This number doesn't adhere to the match rules, so remove it
                        nit = cit->numbers.erase(nit);
                    } else {
                        ++nit;
                    }
                }
                if (cit->numbers.isEmpty()) {
                    // This contact doesn't adhere to the match rules, so remove it
                    contacts.erase(cit);
                } else {
                    ++cit;
                }
            }
            if (contacts.isEmpty()) {
                // If there are no more left break up searching
                break;
            }
        }

        // If one contacte found set callid info
        if (contacts.size() == 1) {
            callID.name = contacts.at(0).name;
            callID.picture = contacts.at(0).picture;
        }
    }

    d->callerID->setCallerID(callID);
}

CallerIDObject *AddressbookModel::callerID() const
{
    Q_ASSERT(d->callerID);
    return d->callerID;
}

}
