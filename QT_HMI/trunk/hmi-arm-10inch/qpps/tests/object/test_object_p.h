#ifndef TEST_OBJECT_P_H
#define TEST_OBJECT_P_H

#include <QTest>

#include <QDebug>
#include <QFile>

#include "object.h"

using namespace QPps;

class ObjectTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanup();

    void testBasicPublishSubscribe();
    void testPublishAndSubscribeInSingleObject();
    void testNoPersistParsing();
    void testAttributesRemove();
    void testDelta();
    void testCache1();
    void testCache2();
    void testNonAscii();

public Q_SLOTS:
    void attributeChanged(const QString &name, const QPps::Variant &value);
    void attributeRemoved(const QString &name);

private:
    void removeObjectFile();
    void connectObject(Object *ppso);
    void connectObjectRemove(Object *ppso);

    bool verifyAttributes(const char* attributes[]) const;

    QString m_ppsObjectFilePath;
    struct AttributeInfo
    {
        QString name;
        Variant value;
    };
    QList<AttributeInfo> m_attributes;
};

#endif
