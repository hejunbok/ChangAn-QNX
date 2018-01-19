#ifndef TEST_VARIANT_P_H
#define TEST_VARIANT_P_H

#include <QTest>

#include <QDebug>
#include <QFile>

class VariantTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanup();
    void testConstruction();
    void testConversions();
};

#endif
