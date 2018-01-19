#include "test_variant_p.h"

#include "variant.h"

#include <QJsonArray>
#include <QJsonDocument>

using namespace QPps;

void VariantTest::initTestCase()
{
}

void VariantTest::cleanup()
{
}

void VariantTest::testConstruction()
{
    const Variant invalid;
    QCOMPARE(invalid, invalid);
    QVERIFY(!invalid.isValid());
    QCOMPARE(invalid.value(), QByteArray());
    QCOMPARE(invalid.encoding(), QByteArray());
    QCOMPARE(invalid.toString(), QString());
    QVERIFY(invalid.toString().isNull());

    const Variant vcustom(QByteArrayLiteral("foo"), QByteArrayLiteral("customEncoding"));
    QCOMPARE(vcustom, vcustom);
    QVERIFY(vcustom != invalid);
    QVERIFY(vcustom.isValid());
    QCOMPARE(vcustom.value(), QByteArrayLiteral("foo"));
    QCOMPARE(vcustom.encoding(), QByteArrayLiteral("customEncoding"));

    const Variant vdouble(5.0);
    QCOMPARE(vdouble, vdouble);
    QVERIFY(vdouble != invalid);
    QVERIFY(vdouble.isValid());
    QCOMPARE(vdouble.encoding(), QByteArrayLiteral("n"));
    QCOMPARE(vdouble.toJson(), QJsonDocument());
    bool ok;
    const double rd = vdouble.toDouble(&ok);
    QVERIFY(ok);
    QCOMPARE(rd, 5.0);

    const Variant vint(5);
    QCOMPARE(vint, vint);
    QVERIFY(vint != invalid);
    QVERIFY(vint.isValid());
    QCOMPARE(vint.encoding(), QByteArrayLiteral("n"));
    QCOMPARE(vint.toJson(), QJsonDocument());
    const int ri = vint.toInt(&ok);
    QVERIFY(ok);
    QCOMPARE(ri, 5);

    const Variant vba(QByteArrayLiteral("test\nline"));
    QCOMPARE(vba, vba);
    QVERIFY(vba != invalid);
    QVERIFY(vba.isValid());
    QCOMPARE(vba.encoding(), QByteArrayLiteral("b64"));
    QCOMPARE(vba.value(), QByteArrayLiteral("dGVzdApsaW5l"));
    QCOMPARE(vba.toByteArray(), QByteArrayLiteral("test\nline"));
    QCOMPARE(vba.toJson(), QJsonDocument());

    const Variant vstr(QStringLiteral("test"));
    QCOMPARE(vstr, vstr);
    QVERIFY(vstr != invalid);
    QVERIFY(vstr.isValid());
    QVERIFY(vstr.encoding().isEmpty());
    QCOMPARE(vstr.toString(), QStringLiteral("test"));
    QCOMPARE(vstr.toByteArray(), QByteArray::fromHex("B5EB2D")); //toByteArray assumes base64 and tries to decode
    QCOMPARE(vstr.toJson(), QJsonDocument());

    const Variant vbool(true);
    QCOMPARE(vbool, vbool);
    QVERIFY(vbool != invalid);
    QVERIFY(vbool.isValid());
    QCOMPARE(vbool.toString(), QStringLiteral("true"));
    QCOMPARE(vbool.encoding(), QByteArrayLiteral("b"));
    QCOMPARE(vbool.toJson(), QJsonDocument());
    QCOMPARE(vbool.toBool(), true);

    QJsonDocument doc;
    QJsonArray array;
    array.append(QJsonValue(5));
    array.append(QJsonValue(QStringLiteral("foobar")));
    doc.setArray(array);

    const Variant vjson(doc);
    QCOMPARE(vjson, vjson);
    QVERIFY(vjson != invalid);
    QVERIFY(vjson.isValid());
    QCOMPARE(vjson.encoding(), QByteArrayLiteral("json"));

    QJsonParseError parseError;
    const QJsonDocument deserializedDoc = vjson.toJson(&parseError);
    QCOMPARE(parseError.error, QJsonParseError::NoError);
    QCOMPARE(doc, deserializedDoc);
}

void VariantTest::testConversions()
{
    //toInt()/toDouble() doesn't work on byte array literals, as they are b64-encoded
    const Variant vbaNum(QByteArrayLiteral("5"));
    QCOMPARE(vbaNum.encoding(), QByteArrayLiteral("b64"));
    bool ok;
    QCOMPARE(vbaNum.toInt(&ok), 0);
    QVERIFY(!ok);
    QCOMPARE(vbaNum.toDouble(&ok), 0.0);
    QVERIFY(!ok);

    //toInt()/toDouble() works just fine when wrapping QStrings though
    const Variant vstrNum(QStringLiteral("5"));
    QVERIFY(vstrNum.encoding().isEmpty());
    QCOMPARE(vstrNum.toInt(&ok), 5);
    QVERIFY(ok);
    QCOMPARE(vstrNum.toDouble(&ok), 5.0);
    QVERIFY(ok);

    //"true" will be converted to true
    const Variant vstrBool(QStringLiteral("true"));
    QCOMPARE(vstrBool.toBool(), true);
}

QTEST_MAIN(VariantTest)
