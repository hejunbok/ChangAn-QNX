#include <QtTest/QtTest>

#include "object.h"
#include "simulator.h"

using namespace QPps;

static void initDemoData()
{
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("id"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("fullName"), QByteArrayLiteral("John Doe"), QByteArrayLiteral("s"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("avatar"), QByteArrayLiteral("male1"), QByteArrayLiteral("s"));
}

class SimulatorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void simulatorObject_publishModes();
    void simulatorObject_withoutAttributeCache();
};

/// This PPS Object has attributes such as 'id', ...
static const QString PPS_PATH = QStringLiteral("/pps/qnxcar/profile/user");

void SimulatorTest::simulatorObject_publishModes()
{
    QPps::Simulator::self()->reset();
    initDemoData();

    {
        QPps::Object object(PPS_PATH, QPps::Object::SubscribeMode);
        QCOMPARE(object.path(), PPS_PATH);
        object.setAttributeCacheEnabled(true);
        QVERIFY(object.attributeCacheEnabled());
        // check write
        QVERIFY(!object.setAttribute(QStringLiteral("foo"), QPps::Variant("bar")));
        QVERIFY(!object.errorString().isEmpty());
        // check read of saved attribute
        QVERIFY(!object.attribute(QStringLiteral("foo")).isValid());
        // check read
        QVERIFY(object.attribute(QStringLiteral("id")).isValid());

        //try remove (must fail in subscribe mode)
        QVERIFY(!object.removeAttribute(QStringLiteral("id")));
        QVERIFY(!object.errorString().isEmpty());
    }

    QPps::Simulator::self()->reset();
    initDemoData();

    {
        QPps::Object object(PPS_PATH, QPps::Object::PublishMode);
        QCOMPARE(object.path(), PPS_PATH);
        object.setAttributeCacheEnabled(true);
        QVERIFY(object.attributeCacheEnabled());
        // check write
        QVERIFY(object.setAttribute(QStringLiteral("foo"), QPps::Variant("bar")));
        QVERIFY(object.errorString().isEmpty());
        // check read of saved attribute, note: we cannot read in PublishMode
        QVERIFY(!object.attribute(QStringLiteral("foo")).isValid());
        // check read
        QVERIFY(!object.attribute(QStringLiteral("id")).isValid());

        //check remove of saved attribute
        QVERIFY(object.removeAttribute(QStringLiteral("foo")));
        QVERIFY(object.errorString().isEmpty());
    }

    QPps::Simulator::self()->reset();
    initDemoData();

    {
        QPps::Object object(PPS_PATH, QPps::Object::PublishAndSubscribeMode);
        QCOMPARE(object.path(), PPS_PATH);
        object.setAttributeCacheEnabled(true);
        QVERIFY(object.attributeCacheEnabled());
        // check writeas
        QVERIFY(object.setAttribute(QStringLiteral("foo"), QPps::Variant("bar")));
        QVERIFY(object.errorString().isEmpty());
        // check read of saved attribute
        QVERIFY(object.attribute(QStringLiteral("foo")).isValid());
        QCOMPARE(object.attribute(QStringLiteral("foo")), QPps::Variant("bar"));
        // check read of original attributes
        QVERIFY(object.attribute(QStringLiteral("id")).isValid());
        QCOMPARE(object.attribute(QStringLiteral("id")), QPps::Variant(1));

        //check remove of saved attribute
        QVERIFY(object.removeAttribute(QStringLiteral("foo")));
        QVERIFY(object.errorString().isEmpty());
        QVERIFY(!object.attribute(QStringLiteral("foo")).isValid());
    }
}


void SimulatorTest::simulatorObject_withoutAttributeCache()
{
    Simulator::self()->reset();
    initDemoData();

    Object object(PPS_PATH, Object::PublishAndSubscribeMode);
    QCOMPARE(object.path(), PPS_PATH);
    QVERIFY(!object.attributeCacheEnabled());

    // check write
    QVERIFY(object.setAttribute(QStringLiteral("foo"), Variant("bar")));
    QVERIFY(object.errorString().isEmpty());
    // check read of saved attribute
    QVERIFY(!object.attribute(QStringLiteral("foo")).isValid());
    // check read (nothing is cached!)
    QVERIFY(!object.attribute(QStringLiteral("id")).isValid());
    QCOMPARE(object.attribute(QStringLiteral("id"), Variant("default")), Variant("default"));

    //check remove of saved attribute
    QVERIFY(object.removeAttribute(QStringLiteral("foo")));
    QVERIFY(object.errorString().isEmpty());
}

QTEST_MAIN(SimulatorTest)
#include "test_simulator.moc"
