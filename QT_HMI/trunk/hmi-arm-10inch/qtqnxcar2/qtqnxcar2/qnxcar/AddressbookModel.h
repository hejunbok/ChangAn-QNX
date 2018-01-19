#ifndef QTQNXCAR2_ADDRESSBOOKMODEL_H
#define QTQNXCAR2_ADDRESSBOOKMODEL_H

#include "qtqnxcar2_export.h"

#include <qnxcar/SqlQueryModel.h>

namespace QnxCar {

class CallerIDObject;

/**
 * Model listing contacts from the address book connected via Bluetooth.
 *
 * This model lists the contacts available in the phonebook database (phonebook.db)
 *
 * To contain data, a bluetooth device supporting the phonebook profile must be connected.
 * The model will be empty if no device is connected.
 */

class QTQNXCAR2_EXPORT AddressbookModel : public SqlQueryModel
{
    Q_OBJECT
    /**
     * Wrapper object filled with the callerID contact information
     *
     * @accessors callerID()
     * @sa #callerID
     */
    Q_PROPERTY(QnxCar::CallerIDObject* callerID READ callerID CONSTANT)
public:
    /// Item roles
    enum Role {
        /// Contact Identifier, role name @c "contact_id"
        ContactIdRole = Qt::UserRole + 1,
        /// Person's last name, role name @c "last_name"
        LastNameRole,
        /// Person's first nane, role name @c "first_name"
        FirstNameRole,
        ///Information fields formatted as a single name string, role name @c "formatted_name"
        FormattedNameRole,
        /// Person's birthday, role name @c "birthday"
        BirthdayRole,
        /// Anniversary date, role name @c "anniversary"
        AnniversaryRole,
        /// Company name, role name @c "company"
        CompanyRole,
        /// Job title, role name @c "job_title"
        JobTitleRole,
        /// First home phone number, role name @c "home_phone"
        HomePhone1Role,
        /// Second home phone number, role name @c "home_phone2"
        HomePhone2Role,
        /// First work phone number, role name @c "work_phone"
        WorkPhone1Role,
        /// Second work phone number, role name @c "work_phone2"
        WorkPhone2Role,
        /// Mobile phone number, role name @c "mobile_phone"
        MobilePhoneRole,
        /// Page number, role name @c "pager_phone"
        PagerPhoneRole,
        /// FAX number, role name @c "fax_phone"
        FaxRole,
        /// Other phone number, role name @c "other_phone"
        OtherPhoneRole,
        /// First email address, role name @c "email_1"
        Email1Role,
        /// Second email address, role name @c "email_2"
        Email2Role,
        /// Third email address, role name @c "email_2"
        Email3Role,
        /// Home address (street), role name @c "home_address_1"
        HomeAddressStreetRole,
        /// Home address (additional info), role name @c "home_address_2"
        HomeAddressExtendedRole,
        /// Home address (city name), role name @c "home_address_city"
        HomeAddressCityRole,
        /// Home address (country name), role name @c "home_address_country"
        HomeAddressCountryRole,
        /// Home address (state/province name), role name @c "home_address_state_province"
        HomeAddressProvinceRole,
        /// Home address (ZIP code), role name @c "home_address_zip_postal"
        HomeAddressZipCodeRole,
        /// Work address (street), role name @c "work_address_1"
        WorkAddressStreetRole,
        /// Work address (additional info), role name @c "work_address_2"
        WorkAddressExtendedRole,
        /// Work address (city name), role name @c "work_address_city"
        WorkAddressCityRole,
        /// Work address (country name), role name @c "work_address_country"
        WorkAddressCountryRole,
        /// Work address (state/province), role name @c "work_address_state_province"
        WorkAddressProvinceRole,
        /// Work address (ZIP code), role name @c "work_address_zip_postal"
        WorkAddressZipCodeRole,
        /// Contact picture, role name @c "picture"
        PictureRole,
        /// PIN, role name @c "pin"
        PinRole,
        /// Person's home page, role name @c "web_page"
        WebPageRole,
        /// Note about person, role name @c "note"
        NoteRole,
        /// Home address fields formatted as a single multiline string, role name @c "formatted_home_address"
        FormattedHomeAddress,
        /// Work address fields formatted as a single multiline string, role name @c "formatted_work_address"
        FormattedWorkAddress,
        /// Person's name as determined by selecting available information from the database, role name @c "display_name"
        DisplayName

    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit AddressbookModel(QObject *parent = 0);

    ~AddressbookModel();

    /**
     * Maps #Role values to names
     */
    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    Q_INVOKABLE void setCallerIDString(const QString &callerIDString);

    CallerIDObject *callerID() const;

protected:
    void runQuery() Q_DECL_OVERRIDE;
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;

private:
    struct Private;
    Private *const d;
};

}

#endif
