#ifndef KEYBOARDPROCESS_H
#define KEYBOARDPROCESS_H

#include <QProcess>

#include <qpps/object.h>

class QStringList;

class WindowManager;

class KeyboardProcess : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardProcess(WindowManager* manager, QObject *parent = 0);
    ~KeyboardProcess();

    void start(const QString &executable, const QStringList &arguments);
    bool waitForStarted(int timeout = 30000);

    Q_PID pid() const;
    QString errorString() const;

public Q_SLOTS:
    void kill();

private Q_SLOTS:
    void standardOutputReadyRead();
    void standardErrorReadyRead();
    void processFinished(int exitCode, QProcess::ExitStatus status);
    /* Handles writing of hidden status of the keyboard to PPS */
    void writeStatusHidden();

private:
    QProcess m_process;
    QString m_executable;
    WindowManager* m_windowManager;
    QPps::Object *ppsStatus;
};

#endif
