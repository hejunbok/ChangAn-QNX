#ifndef EXTERNALAPPLICATION_H
#define EXTERNALAPPLICATION_H

#include <QObject>
#include <QProcess>

namespace QnxCar {
    class Launcher;
}

class ExternalWindow;

class ExternalApplication : public QObject
{
    Q_OBJECT
public:
    explicit ExternalApplication(QnxCar::Launcher *launcher, QObject *parent = 0);

    Q_PID pid() const;

    void setWindow(ExternalWindow *window);

    void start(const QString &appId);

Q_SIGNALS:
    void started();
    void failedToStart();

public Q_SLOTS:
    void windowCreated(Q_PID);
    void windowPosted(Q_PID);

private:
    QnxCar::Launcher *m_launcher;
    Q_PID m_pid;
    bool m_pidReceived;
    ExternalWindow *m_window;

private Q_SLOTS:
    void onAppStartFailed();
};

#endif
