#include <QtTest/QtTest>

#include "util/ModelMonitor.h"

#include <QStringListModel>

using QnxCar::ModelMonitor;

class tst_ModelMonitor : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void modelMonitor_Initial();
    void modelMonitor_AddAlreadyFilledModel();
    void modelMonitor_ResetModel();
    void modelMonitor_DeleteModel();
    void modelMonitor_ChangeModel();
};

void tst_ModelMonitor::modelMonitor_Initial()
{
    QStringListModel model;

    ModelMonitor monitor;
    monitor.setModel(&model);

    QSignalSpy spy(&monitor, SIGNAL(countChanged()));
    QVERIFY(spy.isValid());

    // add element
    QVERIFY(model.insertRows(0, 1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(monitor.count(), 1);

    spy.clear();

    // remove element
    QVERIFY(model.removeRows(0, 1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(monitor.count(), 0);
}

void tst_ModelMonitor::modelMonitor_AddAlreadyFilledModel()
{
    QStringListModel model;
    model.setStringList(QStringList() << QStringLiteral("foo") << QStringLiteral("bar"));
    QCOMPARE(model.rowCount(), 2);

    ModelMonitor monitor;
    monitor.setModel(&model);
    QCOMPARE(monitor.count(), 2);
}

void tst_ModelMonitor::modelMonitor_ResetModel()
{
    QStringListModel model;
    QCOMPARE(model.rowCount(), 0);

    ModelMonitor monitor;
    monitor.setModel(&model);
    QCOMPARE(monitor.count(), 0);

    // be sure the underlying model actually emitted a reset
    // (not the focus of this test, though)
    QSignalSpy resetSpy(&model, SIGNAL(modelReset()));
    model.setStringList(QStringList() << QStringLiteral("foo") << QStringLiteral("bar"));
    QCOMPARE(resetSpy.count(), 1);

    QCOMPARE(monitor.count(), 2);
}

void tst_ModelMonitor::modelMonitor_DeleteModel()
{
    QStringListModel* model = new QStringListModel;
    model->setStringList(QStringList() << QStringLiteral("foo") << QStringLiteral("bar"));

    ModelMonitor monitor;
    monitor.setModel(model);

    QCOMPARE(monitor.model(), model);
    QCOMPARE(monitor.count(), 2);

    QSignalSpy spy(&monitor, SIGNAL(modelChanged()));
    QVERIFY(spy.isValid());

    delete model;

    QCOMPARE(spy.count(), 1);
    QVERIFY(!monitor.model());
    QCOMPARE(monitor.count(), 0);
}

void tst_ModelMonitor::modelMonitor_ChangeModel()
{
    QStringListModel model1;
    QStringListModel model2;
    model2.setStringList(QStringList() << QStringLiteral("foo") << QStringLiteral("bar"));

    ModelMonitor monitor;

    // set initial model
    monitor.setModel(&model1);
    QCOMPARE(monitor.model(), &model1);

    QSignalSpy changeSpy(&monitor, SIGNAL(modelChanged()));
    QVERIFY(changeSpy.isValid());

    // switch model
    QSignalSpy countSpy(&monitor, SIGNAL(countChanged()));
    QVERIFY(countSpy.isValid());

    monitor.setModel(&model2);
    QCOMPARE(changeSpy.count(), 1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(monitor.model(), &model2);
    QCOMPARE(monitor.count(), 2);

    countSpy.clear();

    // now add an element to the first model to check
    // if the monitor disconnected properly from signals
    QVERIFY(model1.insertRows(0, 1));
    QVERIFY(model1.removeRows(0, 1));

    // the count should not have been changed
    QCOMPARE(countSpy.count(), 0);

    // reset model
    changeSpy.clear();
    monitor.setModel(0);
    QCOMPARE(changeSpy.count(), 1);
    QVERIFY(monitor.model() == 0);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(monitor.count(), 0);

    countSpy.clear();

    // now add an element to the second model to check
    // if the monitor disconnected properly from signals
    QVERIFY(model2.insertRows(0, 1));
    QVERIFY(model2.removeRows(0, 1));

    // the count should not have been changed
    QCOMPARE(countSpy.count(), 0);
}

QTEST_MAIN(tst_ModelMonitor)
#include "tst_modelmonitor.moc"
