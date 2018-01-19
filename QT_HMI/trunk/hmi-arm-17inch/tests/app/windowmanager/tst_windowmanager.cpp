#include <QtTest/QtTest>

#include <WindowManager.h>

#include <QSignalSpy>
#include <QString>
#include <QThread>

#include <iostream>

static const QString TEST_APPLICATION = QStringLiteral("gles2-gears");

class tst_WindowManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void testWindowManagerSetProperties();
    void testWindowManagerSignals();

    void cleanupTestCase();

private:
    WindowManager windowManager;
    QThread windowManagerThread;
};

void tst_WindowManager::initTestCase()
{
    QVERIFY(windowManager.init());

    windowManagerThread.moveToThread(&windowManagerThread);
    windowManager.moveToThread(&windowManagerThread);
    QObject::connect(&windowManager, SIGNAL(quit(int,QString)),
                     &windowManagerThread, SLOT(quit()), Qt::QueuedConnection);
    windowManagerThread.start();
    QMetaObject::invokeMethod(&windowManager, "run", Qt::QueuedConnection);
}

void tst_WindowManager::cleanupTestCase()
{
    windowManager.requestQuit();
    windowManagerThread.wait();
}

/**
 * The demo application should show up in the top-right corner with a size of (200,200)
 */
void tst_WindowManager::testWindowManagerSetProperties()
{
    QTest::qWait(100);

    QProcess process;
    process.start(TEST_APPLICATION);

    QVERIFY(process.waitForStarted(5000));

    Q_PID pid = process.pid();

    // FIXME: Hack proposed by Andy -- will be fixed in upcoming Qt versions
    // Using QGuiApplication causes the QNX platform support to render a black root window
    // We have to set a ZValue higher than some magic value to be above that root window
    windowManager.setZValue(pid, 100);

    QTest::qWait(1000);
    windowManager.setSize(pid, QSize(200, 200));

    QTest::qWait(1000);
    windowManager.setPosition(pid, QPoint(100, 100));

    QTest::qWait(1000);
    windowManager.setAlignment(pid, Qt::AlignRight);

    QTest::qWait(1000);
    windowManager.setAlignment(pid, Qt::AlignBottom);

    QTest::qWait(1000);

    process.kill();
    QVERIFY(process.waitForFinished(5000));
}

void tst_WindowManager::testWindowManagerSignals()
{
    // check windowPosted signal
    QSignalSpy createdSpy(&windowManager, SIGNAL(windowPosted(Q_PID)));

    QProcess process;
    process.start(TEST_APPLICATION);

    QVERIFY(process.waitForStarted(5000));

    QTest::qWait(1000); // process events

    QCOMPARE(createdSpy.count(), 1);

    // check windowClosed signal
    QSignalSpy closedSpy(&windowManager, SIGNAL(windowClosed(Q_PID)));

    process.kill();
    QVERIFY(process.waitForFinished());

    QTest::qWait(1000); // process events

    QCOMPARE(closedSpy.count(), 1);
}

QTEST_MAIN(tst_WindowManager)
#include "tst_windowmanager.moc"
