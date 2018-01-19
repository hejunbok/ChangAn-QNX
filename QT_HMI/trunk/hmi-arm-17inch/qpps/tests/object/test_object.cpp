#include "test_object_p.h"

void ObjectTest::attributeChanged(const QString &name, const Variant &value)
{
    AttributeInfo ai = {name, value};
    m_attributes.append(ai);
}

void ObjectTest::attributeRemoved(const QString &name)
{
    QList<AttributeInfo>::ConstIterator it = m_attributes.constBegin();
    int i = 0;
    for (; it != m_attributes.constEnd(); ++it, ++i) {
        const AttributeInfo& ai = *it;
        if(ai.name == name) {
            m_attributes.removeAt(i);
            break;
        }
    }
}

void ObjectTest::connectObject(Object *ppso)
{
    connect(ppso, SIGNAL(attributeChanged(QString, QPps::Variant)),
            this, SLOT(attributeChanged(QString, QPps::Variant)));
}

void ObjectTest::connectObjectRemove(Object *ppso)
{
    connect(ppso, SIGNAL(attributeRemoved(QString)),
            this, SLOT(attributeRemoved(QString)));
}

bool ObjectTest::verifyAttributes(const char* attributes[]) const
{
    int i;
    for (i = 0; ; i += 2) {
        const char *name = attributes[i];
        if (!name)
            break;
        if (i / 2 >= m_attributes.count())
            return false;
        const char *value = attributes[i + 1];
        if (!value)
            return false; // an attribute-value pair without value is invalid

        if (m_attributes[i / 2].name != QString::fromUtf8(name))
            return false;
        if (m_attributes[i / 2].value.toString() != QString::fromUtf8(value))
            return false;
    }
    return i / 2 == m_attributes.count();
}

void ObjectTest::removeObjectFile()
{
    QFile::remove(m_ppsObjectFilePath);
}

void ObjectTest::initTestCase()
{
    m_ppsObjectFilePath = QString::fromLatin1("/pps/applications/__qppstest__");
    removeObjectFile();
}

void ObjectTest::cleanup()
{
    removeObjectFile();
    m_attributes.clear();
}

/**
 * Function to test of removal attributes in PPS object
 * @brief creates then removes 2 attrutes in PPS object
 */
void ObjectTest::testAttributesRemove()
{

    // to monitor attributes changes
    Object subscriber(m_ppsObjectFilePath, Object::SubscribeMode, true);
    QVERIFY(subscriber.isValid());
    subscriber.setAttributeCacheEnabled(false);
    connectObject(&subscriber);
    connectObjectRemove(&subscriber);

    Object publisher(m_ppsObjectFilePath, Object::PublishMode, true);
    QVERIFY(publisher.isValid());
    publisher.setAttributeCacheEnabled(false);
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));
    publisher.setAttribute(QLatin1String("foo2"), Variant(QLatin1String("bar2")));

    QTest::qWait(50);
    static const char *attr[] = {"foo", "bar", "foo2", "bar2", 0};
    QVERIFY(verifyAttributes(attr));

    publisher.removeAttribute(QLatin1String("foo"));

    QTest::qWait(50);
    static const char *attr2[] = {"foo2", "bar2", 0};
    QVERIFY(verifyAttributes(attr2));

    publisher.removeAttribute(QLatin1String("foo2"));
    QTest::qWait(50);
    static const char *attr3[] = {0};
    QVERIFY(verifyAttributes(attr3));
}

void ObjectTest::testBasicPublishSubscribe()
{
    // Open a nonexistent PPS object for reading, but it still works - we get notified when
    // somebody else writes into the object. This is a good thing, because subscribing without a
    // pre-existing publisher could be problematic otherwise.
    Object subscriber(m_ppsObjectFilePath, Object::SubscribeMode, true);
    QVERIFY(subscriber.isValid());
    connectObject(&subscriber);

    Object publisher(m_ppsObjectFilePath, Object::PublishMode, true);
    QVERIFY(publisher.isValid());
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));

    QTest::qWait(50);
    static const char *attr[] = {"foo", "bar", 0};
    QVERIFY(verifyAttributes(attr));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());
}

void ObjectTest::testPublishAndSubscribeInSingleObject()
{
    Object first(m_ppsObjectFilePath,Object::PublishAndSubscribeMode, true);
    QVERIFY(first.isValid());
    first.setAttribute(QLatin1String("hello"), Variant(QLatin1String("world")));

    Object second(m_ppsObjectFilePath, Object::PublishAndSubscribeMode, true);
    QVERIFY(second.isValid());
    connectObject(&second);

    first.setAttribute(QLatin1String("FOO"), Variant(QLatin1String("BAR")));
    QTest::qWait(50);

    // it is rather strange that we receive the FOO::bar change twice
    static const char *attr[] = {"FOO", "BAR", "hello", "world", "FOO", "BAR", 0};
    QVERIFY(verifyAttributes(attr));

    // now swap reader and writer roles
    second.disconnect(this);
    connectObject(&first);

    second.setAttribute(QLatin1String("this"), Variant(QLatin1String("also works")));
    QTest::qWait(50);

    // "first" receives a full update upon "second"'s attribute setting
    static const char *attr2[] = {"FOO", "BAR", "hello", "world",
                                  "FOO", "BAR", "this", "also works", 0};
    QVERIFY(verifyAttributes(attr2));

    QVERIFY(first.errorString().isEmpty());
    QVERIFY(second.errorString().isEmpty());
}

void ObjectTest::testNoPersistParsing()
{
    // this tests that the parsing code can deal with so-called qualifiers (letters in square braces
    // like [ni]) - it does not test that that data is interpreted, because it currently isn't
    Object subscriber(m_ppsObjectFilePath + QLatin1String("?nopersist"), Object::SubscribeMode, true);
    QVERIFY(subscriber.isValid());
    connectObject(&subscriber);

    Object publisher(m_ppsObjectFilePath + QLatin1String("?nopersist"), Object::PublishMode, true);
    QVERIFY(publisher.isValid());
    publisher.setAttribute(QLatin1String("nopersistFoo"), Variant(QLatin1String("nopersistBar")));

    QTest::qWait(50);
    static const char *attr[] = {"nopersistFoo", "nopersistBar", 0};
    QVERIFY(verifyAttributes(attr));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());
}

void ObjectTest::testDelta()
{
    // nopersist is not really required for this test, it's just a small extra difficulty
    Object subscriber(m_ppsObjectFilePath + QLatin1String("?nopersist,delta"), Object::SubscribeMode, true);
    QVERIFY(subscriber.isValid());
    connectObject(&subscriber);

    // another small difficulty: the publisher is read-write
    Object publisher(m_ppsObjectFilePath + QLatin1String("?nopersist"));
    QVERIFY(publisher.isValid());
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));

    QTest::qWait(50);
    // *every* call to setAttribute should cause an update
    static const char *attr[] = {"foo", "bar", "foo", "bar", 0};
    QVERIFY(verifyAttributes(attr));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());
}

void ObjectTest::testCache1()
{
    Object publisher(m_ppsObjectFilePath + QLatin1String("?nopersist"),Object::PublishAndSubscribeMode, true);
    QVERIFY(publisher.isValid());
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));

    Object subscriber(m_ppsObjectFilePath + QLatin1String("?nopersist"),Object::PublishAndSubscribeMode, true);
    QVERIFY(subscriber.isValid());
    subscriber.setAttributeCacheEnabled(true);
    // Test that the caching object fills itself without an even loop pass
    // - unfortunately that means that the changed() signals are emitted from the constructor before
    // we could connect them
    QCOMPARE(subscriber.attribute(QLatin1String("foo")), Variant(QLatin1String("bar")));
    static const char *attr[] = {0};
    QVERIFY(verifyAttributes(attr));

    connectObject(&subscriber);
    QTest::qWait(50); // this should do nothing (the constructor of subscriber already read the updates)
    publisher.setAttribute(QLatin1String("FOO"), Variant(QLatin1String("BAR")));
    QTest::qWait(50);

    QCOMPARE(subscriber.attribute(QLatin1String("FOO")), Variant(QLatin1String("BAR")));
    static const char *attr2[] = {"FOO", "BAR", 0};
    QVERIFY(verifyAttributes(attr2));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());
}

void ObjectTest::testCache2()
{
    // constructing subscriber and publisher in reverse order as previous test, to increase variety
    Object subscriber(m_ppsObjectFilePath + QLatin1String("?nopersist"),Object::PublishAndSubscribeMode, true);
    QVERIFY(subscriber.isValid());
    subscriber.setAttributeCacheEnabled(true);
    connectObject(&subscriber);

    Object publisher(m_ppsObjectFilePath + QLatin1String("?nopersist"),Object::PublishAndSubscribeMode, true);
    QVERIFY(publisher.isValid());
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));
    publisher.setAttribute(QLatin1String("foo"), Variant(QLatin1String("bar")));

    static const char *attr[] = {0};
    QVERIFY(verifyAttributes(attr));
    QTest::qWait(50);
    QCOMPARE(subscriber.attribute(QLatin1String("foo")), Variant(QLatin1String("bar")));
    static const char *attr2[] = {"foo", "bar", "foo", "bar", 0};
    QVERIFY(verifyAttributes(attr2));

    publisher.setAttribute(QLatin1String("cache"), Variant(QLatin1String("works")));
    QTest::qWait(50);
    QCOMPARE(subscriber.attribute(QLatin1String("foo")), Variant(QLatin1String("bar")));
    QCOMPARE(subscriber.attribute(QLatin1String("cache")), Variant(QLatin1String("works")));
    static const char *attr3[] = {"foo", "bar", "foo", "bar", "cache", "works", 0};
    QVERIFY(verifyAttributes(attr3));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());
}

void ObjectTest::testNonAscii()
{
    Object subscriber(m_ppsObjectFilePath, Object::SubscribeMode, true);
    QVERIFY(subscriber.isValid());
    connectObject(&subscriber);

    Object publisher(m_ppsObjectFilePath, Object::PublishMode, true);
    QVERIFY(publisher.isValid());
    publisher.setAttribute(QString::fromUtf8("foo"), Variant(QString::fromUtf8("bär")));

    QTest::qWait(50);
    static const char *attr[] = {"foo", "bär", 0};
    QVERIFY(verifyAttributes(attr));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());

    publisher.setAttribute(QString::fromUtf8("FOO"), Variant(QString::fromUtf8("ÜBÉR")));
    QTest::qWait(50);
    static const char *attr2[] = {"foo", "bär", "FOO", "ÜBÉR", 0};
    QVERIFY(verifyAttributes(attr2));

    QVERIFY(publisher.errorString().isEmpty());
    QVERIFY(subscriber.errorString().isEmpty());

    // non-ASCII attribute names are not allowed, so this change should be ignored
    publisher.setAttribute(QString::fromUtf8("FOOÄ"), Variant(QString::fromUtf8("bar")));
    QTest::qWait(50);
    QVERIFY(verifyAttributes(attr2));

    QVERIFY(!publisher.errorString().isEmpty()); // not empty!
    QVERIFY(subscriber.errorString().isEmpty());
}

QTEST_MAIN(ObjectTest)
