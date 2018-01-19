#include "test_dirwatcher_p.h"

#include "object.h"

#include <QDir>

static bool rmdirRecursive(const QString &dirName)
{
    QDir dir(dirName);
    if (!dir.exists()) {
        return false;
    }
    foreach (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                               QDir::Hidden | QDir::AllDirs | QDir::Files,
                                               QDir::DirsFirst)) {
        if (info.isDir() && !info.isSymLink()) {
            if (!rmdirRecursive(info.absoluteFilePath())) {
                return false;
            }
        } else {
            if (!QFile::remove(info.absoluteFilePath())) {
                return false;
            }
        }
    }
    return dir.rmdir(dir.absolutePath());
}

void DirWatcherTest::objectAdded(const QString &name)
{
    m_objects.insert(name);
    m_objectChanges.append(QStringLiteral("+") + name);
}

void DirWatcherTest::objectRemoved(const QString &name)
{
    m_objects.remove(name);
    m_objectChanges.append(QStringLiteral("-") + name);
}

void DirWatcherTest::connectDirWatcher(DirWatcher *watcher)
{
    connect(watcher, SIGNAL(objectAdded(QString)), this, SLOT(objectAdded(QString)));
    connect(watcher, SIGNAL(objectRemoved(QString)), this, SLOT(objectRemoved(QString)));
}

bool DirWatcherTest::verifyObjects(const char* objects[]) const
{
    int i;
    for (i = 0; ; i++) {
        const char *object = objects[i];
        if (!object)
            break;
        if (!m_objects.contains(QString::fromUtf8(object)))
            return false;
    }
    return i == m_objects.count();
}

bool DirWatcherTest::verifyObjectChanges(const char* changes[]) const
{
    int i;
    for (i = 0; ; i++) {
        const char *change = changes[i];
        if (!change)
            break;
        if (i >= m_objectChanges.count())
            return false;

        if (m_objectChanges[i] != QString::fromUtf8(change))
            return false;
    }
    return i == m_objectChanges.count();
}

void DirWatcherTest::createTestDir()
{
    QDir dir;
    QVERIFY(dir.mkdir(m_ppsDirPath));
}

void DirWatcherTest::removeTestDir()
{
    rmdirRecursive(m_ppsDirPath);
}

void DirWatcherTest::init()
{
    m_ppsDirPath = QString::fromLatin1("/pps/applications/__qppstestdir__");
    createTestDir();
}

void DirWatcherTest::cleanup()
{
    removeTestDir();
    m_objects.clear();
    m_objectChanges.clear();
}

void DirWatcherTest::testAdd()
{
    Object object0(m_ppsDirPath + QStringLiteral("/object0"), Object::PublishMode);
    Object object1(m_ppsDirPath + QStringLiteral("/object1"), Object::PublishMode);

    DirWatcher dw(m_ppsDirPath);
    connectDirWatcher(&dw);
    QTest::qWait(50);

    Object object2(m_ppsDirPath + QStringLiteral("/object2"), Object::PublishMode);
    Object object3(m_ppsDirPath + QStringLiteral("/object3"), Object::PublishMode);

    QTest::qWait(50);

    const char *objects[] = {"object0", "object1", "object2", "object3", 0};
    QVERIFY(verifyObjects(objects));
    const char *objectChanges[] = {"+object0", "+object1", "+object2", "+object3", 0};
    QVERIFY(verifyObjectChanges(objectChanges));
}

void DirWatcherTest::testRemove()
{
    Object object0(m_ppsDirPath + QStringLiteral("/object0"), Object::PublishMode);
    Object object1(m_ppsDirPath + QStringLiteral("/object1"), Object::PublishMode);

    DirWatcher dw(m_ppsDirPath);
    connectDirWatcher(&dw);
    QTest::qWait(50);

    Object object2(m_ppsDirPath + QStringLiteral("/object2"), Object::PublishMode);

    QTest::qWait(50);

    const char *objects[] = {"object0", "object1", "object2", 0};
    QVERIFY(verifyObjects(objects));
    const char *objectChanges[] = {"+object0", "+object1", "+object2", 0};
    QVERIFY(verifyObjectChanges(objectChanges));

    QFile::remove(object1.path());

    QTest::qWait(50);

    const char *objects2[] = {"object0", "object2", 0};
    QVERIFY(verifyObjects(objects2));
    const char *objectChanges2[] = {"+object0", "+object1", "+object2", "-object1", 0};
    QVERIFY(verifyObjectChanges(objectChanges2));

    QFile::remove(object2.path());
    QFile::remove(object0.path());

    QTest::qWait(50);

    const char *objects3[] = {0};
    QVERIFY(verifyObjects(objects3));
    const char *objectChanges3[] = {"+object0", "+object1", "+object2",
                                    "-object1", "-object2", "-object0", 0};
    QVERIFY(verifyObjectChanges(objectChanges3));
}

void DirWatcherTest::testNonAscii()
{
    Object object0(m_ppsDirPath + QStringLiteral("/objectÄ"), Object::PublishMode);

    DirWatcher dw(m_ppsDirPath);
    connectDirWatcher(&dw);
    QTest::qWait(50);

    Object object1(m_ppsDirPath + QStringLiteral("/öbjéctB"), Object::PublishMode);

    QTest::qWait(50);

    const char *objects[] = {"objectÄ", "öbjéctB", 0};
    QVERIFY(verifyObjects(objects));
    const char *objectChanges[] = {"+objectÄ", "+öbjéctB", 0};
    QVERIFY(verifyObjectChanges(objectChanges));

    QFile::remove(object1.path());
    QTest::qWait(50);

    const char *objects2[] = {"objectÄ", 0};
    QVERIFY(verifyObjects(objects2));
    const char *objectChanges2[] = {"+objectÄ", "+öbjéctB", "-öbjéctB", 0};
    QVERIFY(verifyObjectChanges(objectChanges2));
}

QTEST_MAIN(DirWatcherTest)
