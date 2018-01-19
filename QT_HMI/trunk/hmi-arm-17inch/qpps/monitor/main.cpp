#include <QCoreApplication>

#include "ppsmonitor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    PpsMonitor monitor;
    return a.exec();
}
