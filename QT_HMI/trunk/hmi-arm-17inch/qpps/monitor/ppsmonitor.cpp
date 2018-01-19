#include "ppsmonitor.h"

#include "dirmonitor.h"

#include <QDir>


PpsMonitor::PpsMonitor(QObject *parent)
   : QObject(parent)
{
    watchRecursive(QStringLiteral("/pps"));
}

void PpsMonitor::watchRecursive(const QString &dirName)
{
    QDir dir(dirName);
    if (!dir.exists()) {
        return;
    }

    new DirMonitor(dirName, this);

    foreach (const QFileInfo &info, dir.entryInfoList(QDir::Dirs | QDir::AllDirs | QDir::NoSymLinks |
                                                      QDir::NoDotAndDotDot | QDir::Hidden,
                                                      QDir::Name) ) {
        watchRecursive(info.absoluteFilePath());
    }
}
