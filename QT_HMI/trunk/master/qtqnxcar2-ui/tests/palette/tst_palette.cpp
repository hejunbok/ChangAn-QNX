#include <QtTest/QtTest>

#include "qnxcarui/Palette.h"

#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>

using namespace QnxCarUi;

class tst_Palette : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testPaletteParser();
    void testPaletteData();
    void testPaletteItem();
    void testPaletteItem_fromString();
};

void tst_Palette::testPaletteParser()
{
    QString qmlFile = QStringLiteral("%1/PaletteDefinition.qml").arg(QStringLiteral(QML_PATH));

    QQmlEngine engine;
    QQmlComponent component(&engine, qmlFile);
    QVERIFY(!component.isError());
    QVERIFY(component.isReady());

    QObject *object = component.create();
    QVERIFY(object);
    QVariant paletteProp = object->property("palette");
    QVERIFY(paletteProp.isValid());

    PaletteParser parser(paletteProp);
    PaletteData palette = parser.parse();
    QVERIFY(!palette.isEmpty());
    QCOMPARE(palette.color(PaletteItem::DropDown), QColor(Qt::red));
    QCOMPARE(palette.color(PaletteItem::DropDownPressed), QColor(Qt::blue));
    QCOMPARE(palette.color(PaletteItem::PushButton), QColor(Qt::transparent));

    delete object;
}

void tst_Palette::testPaletteItem_fromString()
{
    PaletteItem::ColorRole role = PaletteItem::fromString("DropDown");
    QCOMPARE(role, PaletteItem::DropDown);

    role = PaletteItem::fromString("PushButton");
    QCOMPARE(role, PaletteItem::PushButton);
}

void tst_Palette::testPaletteData()
{
    PaletteData data1;
    QVERIFY(data1.isEmpty());
    PaletteData data2;
    QVERIFY(data2.isEmpty());
    QCOMPARE(data1, data2);

    data1.setColor(PaletteItem::DropDown, QColor(Qt::red));
    QVERIFY(!data1.isEmpty());
    QCOMPARE(data1.color(PaletteItem::DropDown), QColor(Qt::red));

    QVERIFY(data1 != data2);

    data1.unsetColor(PaletteItem::DropDown);
    QCOMPARE(data1, data2);
}

void tst_Palette::testPaletteItem()
{
    PaletteData data;
    data.setColor(PaletteItem::DropDown, QColor(Qt::red));

    PaletteItem item;
    item.setPalette(data);
    QCOMPARE(item.color(PaletteItem::DropDown), QColor(Qt::red));
}

QTEST_MAIN(tst_Palette)
#include "tst_palette.moc"
