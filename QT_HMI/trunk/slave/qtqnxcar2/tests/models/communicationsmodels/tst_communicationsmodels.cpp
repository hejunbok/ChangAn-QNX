#include <QtTest/QtTest>

#include "qnxcar/AddressbookModel.h"
#include "qnxcar/DatabaseManager.h"
#include "qnxcar/MessagesModel.h"
#include "qnxcar/CallerIDObject.h"

#include <QSqlRecord>
#include <QString>

using QnxCar::AddressbookModel;
using QnxCar::DatabaseManager;
using QnxCar::MessagesModel;

class tst_CommunicationsModels : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAddressbookModel();
    void testMessagesModel();
};

void tst_CommunicationsModels::testAddressbookModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("phonebook.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    AddressbookModel model;
    model.setDatabase(db);
    while (model.canFetchMore())
        model.fetchMore();

    QCOMPARE(model.rowCount(), 12);

    QCOMPARE(model.data(model.index(0, 0), AddressbookModel::ContactIdRole).toString(), QStringLiteral("3"));
    QCOMPARE(model.data(model.index(0, 0), AddressbookModel::LastNameRole).toString(), QString());
    QCOMPARE(model.data(model.index(0, 0), AddressbookModel::FirstNameRole).toString(), QStringLiteral("Fhhcv"));

    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::ContactIdRole).toString(), QStringLiteral("4"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::LastNameRole).toString(), QStringLiteral("Maulwurf"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::FirstNameRole).toString(), QStringLiteral("Hans"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::BirthdayRole).toDate(), QDate());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::AnniversaryRole).toDate(), QDate());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::CompanyRole).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::JobTitleRole).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomePhone1Role).toString(), QStringLiteral("03064854726"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomePhone2Role).toString(), QStringLiteral("03064862465"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkPhone1Role).toString(), QStringLiteral("03066667777"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkPhone2Role).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::MobilePhoneRole).toString(), QStringLiteral("080008000800"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::PagerPhoneRole).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::FaxRole).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::OtherPhoneRole).toString(), QStringLiteral("0170425199"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::Email1Role).toString(), QStringLiteral("moleman@simpsons.com"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::Email2Role).toString(), QStringLiteral("moleman@maulwurfag.com"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::Email3Role).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomeAddressStreetRole).toString(), QStringLiteral("Maulwurfallee 13"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomeAddressExtendedRole).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomeAddressCityRole).toString(), QStringLiteral("Berlin"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomeAddressCountryRole).toString(), QStringLiteral("Germany"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::HomeAddressZipCodeRole).toString(), QStringLiteral("12121"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkAddressStreetRole).toString(), QStringLiteral("Maulwurfweg 12"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkAddressExtendedRole).toString(), QStringLiteral("XYZ54"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkAddressCityRole).toString(), QStringLiteral("Berlin "));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkAddressCountryRole).toString(), QStringLiteral("Germany"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkAddressProvinceRole).toString(), QStringLiteral("Berlin"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WorkAddressZipCodeRole).toString(), QStringLiteral("13131"));
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::PictureRole).toByteArray(), QByteArray());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::WebPageRole).toString(), QString());
    QCOMPARE(model.data(model.index(8, 0), AddressbookModel::NoteRole).toString(), QString());

    QCOMPARE(model.data(model.index(11, 0), AddressbookModel::ContactIdRole).toString(), QStringLiteral("7"));
    QCOMPARE(model.data(model.index(11, 0), AddressbookModel::LastNameRole).toString(), QStringLiteral("Saint"));
    QCOMPARE(model.data(model.index(11, 0), AddressbookModel::FirstNameRole).toString(), QStringLiteral("Pater"));

    // Test callid
    model.setCallerIDString(QStringLiteral("0170425199"));
    QCOMPARE(model.callerID()->name(), QStringLiteral("Hans Maulwurf"));
    model.setCallerIDString(QStringLiteral("+49170425199"));
    QCOMPARE(model.callerID()->name(), QStringLiteral("Hans Maulwurf"));
    model.setCallerIDString(QStringLiteral("5551234"));
    QCOMPARE(model.callerID()->name(), QStringLiteral(" BlackBerry Voice Control"));
}

void tst_CommunicationsModels::testMessagesModel()
{
    QSqlDatabase db = DatabaseManager::database(QStringLiteral("messages.db"));;
    QVERIFY(db.open());
    QVERIFY(db.isOpen());

    MessagesModel model;
    model.setDatabase(db);
    while (model.canFetchMore())
        model.fetchMore();

    QCOMPARE(model.rowCount(), 28);

    QCOMPARE(model.data(model.index(0, 0), MessagesModel::MessageIdRole).toString(), QStringLiteral("1"));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::TypeRole).toInt(), static_cast<int>(MessagesModel::EMail));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::SenderLabelRole).toString(), QStringLiteral("QNX CAR"));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::SubjectRole).toString(), QStringLiteral("Test email to iphone 5"));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::DateTimeRole).toDateTime(), QDateTime(QDate(2013, 4, 15), QTime(17, 11, 53)));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::AccountIdRole).toInt(), 1);

    QCOMPARE(model.data(model.index(27, 0), MessagesModel::MessageIdRole).toString(), QStringLiteral("28"));
    QCOMPARE(model.data(model.index(27, 0), MessagesModel::TypeRole).toInt(), static_cast<int>(MessagesModel::SMS));
    QCOMPARE(model.data(model.index(27, 0), MessagesModel::SenderLabelRole).toString(), QStringLiteral("+19026929786"));
    QCOMPARE(model.data(model.index(27, 0), MessagesModel::SubjectRole).toString(), QStringLiteral("Hey. This Bethanie?"));
    QCOMPARE(model.data(model.index(27, 0), MessagesModel::DateTimeRole).toDateTime(), QDateTime(QDate(2013, 3, 24), QTime(19, 2, 53)));
    QCOMPARE(model.data(model.index(27, 0), MessagesModel::AccountIdRole).toInt(), 0);

    model.setFilterMessageType(MessagesModel::EMail);

    QCOMPARE(model.rowCount(), 22);

    QCOMPARE(model.data(model.index(21, 0), MessagesModel::MessageIdRole).toString(), QStringLiteral("22"));
    QCOMPARE(model.data(model.index(21, 0), MessagesModel::TypeRole).toInt(), static_cast<int>(MessagesModel::EMail));
    QCOMPARE(model.data(model.index(21, 0), MessagesModel::SenderLabelRole).toString(), QStringLiteral("Box"));
    QCOMPARE(model.data(model.index(21, 0), MessagesModel::SubjectRole).toString(), QStringLiteral("Misha Lytvynyuk has accepted the invitation to your 'Documents' folder on  Box"));
    QCOMPARE(model.data(model.index(21, 0), MessagesModel::DateTimeRole).toDateTime(), QDateTime(QDate(2013, 3, 24), QTime(22, 18, 3)));
    QCOMPARE(model.data(model.index(21, 0), MessagesModel::AccountIdRole).toInt(), 1);

    model.setFilterMessageType(MessagesModel::SMS);

    QCOMPARE(model.rowCount(), 5);

    QCOMPARE(model.data(model.index(0, 0), MessagesModel::MessageIdRole).toString(), QStringLiteral("23"));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::TypeRole).toInt(), static_cast<int>(MessagesModel::SMS));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::SenderLabelRole).toString(), QStringLiteral("+16132658221"));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::SubjectRole).toString(), QStringLiteral("Sorry wrong number"));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::DateTimeRole).toDateTime(), QDateTime(QDate(2013, 4, 7), QTime(15, 20, 25)));
    QCOMPARE(model.data(model.index(0, 0), MessagesModel::AccountIdRole).toInt(), 0);
}

QTEST_MAIN(tst_CommunicationsModels)
#include "tst_communicationsmodels.moc"
