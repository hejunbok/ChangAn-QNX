#ifndef TEST_DIRWATCHER_P_H
#define TEST_DIRWATCHER_P_H

#include <QTest>
#include <QSet>

#include "dirwatcher.h"

using namespace QPps;

class DirWatcherTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();

    void testAdd();
    void testRemove();
    void testNonAscii();

public Q_SLOTS:
    void objectAdded(const QString &name);
    void objectRemoved(const QString &name);

private:
    void createTestDir();
    void removeTestDir();
    void connectDirWatcher(DirWatcher *watcher);

    bool verifyObjects(const char* objects[]) const;
    bool verifyObjectChanges(const char* objectChanges[]) const;

    QString m_ppsDirPath;
    QSet<QString> m_objects;
    QStringList m_objectChanges;
};

#endif
