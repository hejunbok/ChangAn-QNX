#include "ExternalApplication.h"
#include "ExternalWindow.h"

#include <qnxcar/Launcher.h>
#include <QCoreApplication>

ExternalApplication::ExternalApplication(QnxCar::Launcher *launcher, QObject *parent)
    : QObject(parent)
    , m_launcher(launcher)
    , m_pid()
    , m_pidReceived(false)
    , m_window(0)
{
    Q_ASSERT(m_launcher);

   connect(m_launcher, SIGNAL(applicationStartFailed()), this, SLOT(onAppStartFailed()));
}

void ExternalApplication::windowCreated(Q_PID pid)
{
    //Commented out as under windows this cannot be hit and qApp->applicationPid()
    //returns a qint64 as Q_PID beeing a struct
#ifndef Q_OS_WIN
    if (qApp->applicationPid() == pid)
        return;
#endif
    if (m_pidReceived)
        return;
    m_pid = pid;
    m_pidReceived = true;
    if (m_window)
        m_window->setPid(m_pid);
}

void ExternalApplication::windowPosted(Q_PID pid)
{
    if (pid == m_pid)
        emit started();
}

void ExternalApplication::setWindow(ExternalWindow *window)
{
    connect(this, &ExternalApplication::started, window, &ExternalWindow::handleApplicationStarted);
    m_window = window;
}

void ExternalApplication::start(const QString &appId)
{
    m_launcher->start(appId);
}

void ExternalApplication::onAppStartFailed() {
    //cleanup when applicaiton failed to start
    m_pid = 0;
    m_pidReceived = false;
    emit failedToStart();
}
