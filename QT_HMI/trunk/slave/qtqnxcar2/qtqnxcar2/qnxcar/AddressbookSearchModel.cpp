#include "AddressbookSearchModel.h"

#include <QSqlQuery>
#include <QStringList>

namespace QnxCar {

QList<QByteArray> AddressbookSearchModel::columnNames() const {
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
    "FROM contacts_view "
    "WHERE asr_match_score > 0 "
    "ORDER BY asr_match_score DESC"
);

QString SQL_SELECT_CONTACTS = SQL_SELECT_CONTACTS_TPL + SQL_RECORD_LIMIT;

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
AddressbookSearchModel::AddressbookSearchModel(QObject* parent)
    : SqlQueryModel(parent)
{
}

void AddressbookSearchModel::runQuery()
{
    reopenDatabase();
    setQuery(SQL_SELECT_CONTACTS, database());
}

QHash< int, QByteArray > AddressbookSearchModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = SqlQueryModel::roleNames();
    roleNames.insert(FormattedHomeAddress, "formatted_home_address");
    roleNames.insert(FormattedWorkAddress, "formatted_work_address");
    roleNames.insert(DisplayName, "display_name");
    return roleNames;
}

QVariant AddressbookSearchModel::data(const QModelIndex &index, int role) const
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

}
