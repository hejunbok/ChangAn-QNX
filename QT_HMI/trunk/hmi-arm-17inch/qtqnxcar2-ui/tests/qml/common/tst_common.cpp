#include "qnxcarui/qtresourceinit.h"
#include "qnxcarui/Palette.h"
#include "qnxcarui/ThemeManager.h"

#include <QtTest/QtTest>

#include <QDebug>
#include <QFileInfo>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QSet>

typedef QSet<QByteArray> PropertySet;

class TestPalette : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QnxCarUi::PaletteItem* palette READ palette NOTIFY paletteChanged)

public:
    TestPalette()
        : m_palette(new QnxCarUi::PaletteItem(this))
    {
    }

    QnxCarUi::PaletteItem *palette() const { return m_palette; }

    Q_INVOKABLE QColor color(const QVariant &role) const;

Q_SIGNALS:
    void paletteChanged();

private:
    QnxCarUi::PaletteItem *m_palette;
};

QColor TestPalette::color(const QVariant &role) const
{
    Q_UNUSED(role);
    return Qt::black;
}

class TestThemeManager : public QObject
{
    Q_OBJECT
public:
    TestThemeManager() {
    }

    Q_INVOKABLE QString fromTheme(const QString &) { return QString(); }
};

class tst_Common : public QObject
{
    Q_OBJECT

public:
    tst_Common()
    {
        QnxCarUi::initQtResources();
        qmlRegisterUncreatableType<QnxCarUi::PaletteItem>("com.qnx.car.ui", 2, 0, "Palette", QStringLiteral("Access to object"));

        engine.rootContext()->setContextProperty(QStringLiteral("display"), QString());
        engine.rootContext()->setContextProperty(QStringLiteral("model"), this);

        engine.rootContext()->setContextProperty(QStringLiteral("_palette"), &palette);
        engine.rootContext()->setContextProperty(QStringLiteral("_theme"), &theme);
        engine.rootContext()->setContextProperty(QStringLiteral("_themeManager"), &themeManager);
        engine.rootContext()->setContextProperty(QStringLiteral("_resolutionFactor"), 1.0);
    }

private:
    QQmlEngine engine;
    TestPalette palette;
    QnxCar::Theme theme;
    TestThemeManager themeManager;

private Q_SLOTS:
    void testUtilScript();

    void testQmlComponents_data();
    void testQmlComponents();
};

void tst_Common::testUtilScript()
{
    QVERIFY(QFileInfo(":/qnxcarui/qml/common/util.js").exists());

    QString qmlFile = QStringLiteral("%1/UtilJsTest.qml").arg(QStringLiteral(QML_PATH));

    QQmlComponent component(&engine, qmlFile);
    QVERIFY(!component.isError());
    QVERIFY(component.isReady());

    QObject *object = component.create();
    QVERIFY(object);

    // test iconForDeviceType()
    QVariant iconName;
    QVERIFY(QMetaObject::invokeMethod(object, "iconForDeviceType", Q_RETURN_ARG(QVariant, iconName), Q_ARG(QVariant, "hdd")));
    QCOMPARE(iconName.toString(), QStringLiteral("HDDef"));
    QVERIFY(QMetaObject::invokeMethod(object, "iconForDeviceType", Q_RETURN_ARG(QVariant, iconName), Q_ARG(QVariant, "usb")));
    QCOMPARE(iconName.toString(), QStringLiteral("USB"));
    QVERIFY(QMetaObject::invokeMethod(object, "iconForDeviceType", Q_RETURN_ARG(QVariant, iconName), Q_ARG(QVariant, "")));
    QCOMPARE(iconName.toString(), QStringLiteral("SDCard"));

    // TODO add tests other functions

    delete object;
}

void tst_Common::testQmlComponents_data()
{
    QTest::addColumn<QString>("component");
    QTest::addColumn<PropertySet>("properties");

    QTest::newRow("BasicSprite") << "BasicSprite"
                                 << (PropertySet() << "vertical" << "source" << "currentFrame" << "frameCount");
    QTest::newRow("ButtonArea") << "ButtonArea"
                                << (PropertySet() << "pressed" << "checked" << "checkable");
    QTest::newRow("DialArea") << "DialArea"
                              << (PropertySet() << "stepSize" << "value" << "highlightValue"
                                  << "clickModeEnabled" << "containsMouse" << "inDrag" << "snapSize"
                                  << "activeFocusOnPress");
    QTest::newRow("DoubleLineListDelegate") << "DoubleLineListDelegate"
                                            << (PropertySet() << "modelData");
    QTest::newRow("DoubleLineListEntry") << "DoubleLineListEntry"
                                         << (PropertySet() << "text" << "subText" << "rightText" << "font"
                                             << "subFont" << "iconSourceLeft" << "iconSourceRight" << "maximumIconWidth"
                                             << "maximumIconHeight" << "highlighted" << "textColor" << "highlightColor"
                                             << "highlightTextColor" << "iconContainerRight");
    QTest::newRow("DropdownButton") << "DropdownButton"
                                    << (PropertySet() << "textArea");
    QTest::newRow("MatrixSprite") << "MatrixSprite"
                                  << (PropertySet() << "source" << "currentXFrame" << "currentYFrame"
                                      << "xFrameCount" << "yFrameCount");
    QTest::newRow("MediaProgressBar") << "MediaProgressBar"
                                      << (PropertySet() << "sourceBackground" << "sourceOverlay" << "totalTime"
                                          << "currentTime" << "interactive");
    QTest::newRow("MenuOverlayPane") << "MenuOverlayPane"
                                     << (PropertySet() << "delegate" << "model" << "section" << "spacing"
                                         << "listViewCount");
    QTest::newRow("MultiStateBasicSpriteButton") << "MultiStateBasicSpriteButton"
                                                 << (PropertySet() << "sprite" << "currentState" << "stateCount");
    QTest::newRow("MultiStateMatrixSpriteButton") << "MultiStateMatrixSpriteButton"
                                                  << (PropertySet() << "sprite" << "currentState" << "stateCount");
    QTest::newRow("NavigatorGridElement") << "NavigatorGridElement"
                                          << (PropertySet() << "selected" << "iconPath" << "title");
    QTest::newRow("OverlayPane") << "OverlayPane"
                                 << (PropertySet() << "active" << "selfDeactivation" << "animated" << "content"
                                     << "indentationBase" << "indentationLevel" << "indentationSpacing");
    QTest::newRow("PushButton") << "PushButton"
                                << (PropertySet() << "background" << "label");
    QTest::newRow("PushButtonFromImage") << "PushButtonFromImage"
                                         << (PropertySet() << "background" << "overlay" << "label");
    QTest::newRow("ScreenBackground") << "ScreenBackground"
                                      << PropertySet();
    QTest::newRow("SearchField") << "SearchField"
                                 << (PropertySet() << "placeholderText" << "searchText" << "searchActiveFocus");
    QTest::newRow("SheetMenu") << "SheetMenu"
                               << (PropertySet() << "extended" << "animated" << "sliding" << "content");
    QTest::newRow("SheetMenuDelegate") << "SheetMenuDelegate" << PropertySet();
    QTest::newRow("SimpleListDelegate") << "SimpleListDelegate"
                                        << (PropertySet() << "modelData");
    QTest::newRow("SimpleListEntry") << "SimpleListEntry"
                                     << (PropertySet() << "text" << "font" << "fontSize" << "iconSourceLeft" << "iconSourceRight"
                                         << "maximumIconWidth" << "maximumIconHeight" << "iconMargins" << "textMargins"
                                         << "forceHighlight" << "highlighted"
                                         << "textColor" << "highlightColor" << "highlightTextColor");
    QTest::newRow("Spinner") << "Spinner" << PropertySet();
    QTest::newRow("SpriteButton") << "SpriteButton"
                                  << (PropertySet() << "imageSource" << "imageWidth" << "imageHeight"
                                      << "checkedFrame" << "pressedFrame" << "unpressedFrame" << "sprite");
    QTest::newRow("SpriteButtonTextAndImage") << "SpriteButtonTextAndImage"
                                              << (PropertySet() << "checkedFrame" << "pressedFrame" << "unpressedFrame"
                                                  << "sprite" << "label" << "image");
}

void tst_Common::testQmlComponents()
{
    QFETCH(QString, component);
    QFETCH(PropertySet, properties);

    QList<QByteArray> expectedProperties = properties.toList();
    qSort(expectedProperties);

    QVERIFY(QFileInfo(QStringLiteral(":/qnxcarui/qml/common/%1.qml").arg(component)).exists());

    QString testFile = QStringLiteral("%1/%2Test.qml").arg(QStringLiteral(QML_PATH)).arg(component);

    if (!QFileInfo(testFile).exists()) {
        QUrl qmlUrl = QStringLiteral("qrc:///qnxcarui/qml/common/%1.qml").arg(component);

        QQmlComponent qmlComponent(&engine, qmlUrl);
        QVERIFY(!qmlComponent.isError());
        QVERIFY(qmlComponent.isReady());

        QObject *object = qmlComponent.create();
        QVERIFY(object);

        if (!properties.isEmpty()) {
            PropertySet qmlProperties;
            for (int i = object->metaObject()->propertyOffset(); i < object->metaObject()->propertyCount(); ++i) {
                qmlProperties << object->metaObject()->property(i).name();
            }

            QList<QByteArray> qmlPropertiesList = qmlProperties.toList();
            qSort(qmlPropertiesList);
            QCOMPARE(qmlPropertiesList, expectedProperties);
        }

        delete object;
    } else {
        // component can't be loaded directly due to referencing "parent"

        QQmlComponent testComponent(&engine, testFile);
        QVERIFY(!testComponent.isError());
        QVERIFY(testComponent.isReady());

        QObject *object = testComponent.create();
        QVERIFY(object);

        QObject *innerItem = object->findChild<QObject*>("innerItem");
        QVERIFY(innerItem);

        if (!properties.isEmpty()) {
            PropertySet qmlProperties;
            for (int i = innerItem->metaObject()->propertyOffset(); i < innerItem->metaObject()->propertyCount(); ++i) {
                qmlProperties << innerItem->metaObject()->property(i).name();
            }

            QList<QByteArray> qmlPropertiesList = qmlProperties.toList();
            qSort(qmlPropertiesList);
            QCOMPARE(qmlPropertiesList, expectedProperties);
        }

        delete object;
    }
}

QTEST_MAIN(tst_Common)
#include "tst_common.moc"
