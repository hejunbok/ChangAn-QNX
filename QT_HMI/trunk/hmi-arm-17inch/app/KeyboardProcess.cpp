#include "KeyboardProcess.h"
#include "WindowManager.h"

#include <algorithm>

#include <QDebug>

KeyboardProcess::KeyboardProcess(WindowManager* windowManager, QObject *parent)
    : QObject(parent)
    , m_windowManager(windowManager)
{
    Q_ASSERT(m_windowManager);
    connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(standardErrorReadyRead()));
    connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(standardOutputReadyRead()));
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));

    ppsStatus = new QPps::Object(QStringLiteral("/pps/system/keyboard/status"),QPps::Object::PublishMode, false, this);

    if (!ppsStatus->isValid()) {
        qWarning() << Q_FUNC_INFO << "Could not open PPS status object:" << ppsStatus->errorString();
    }
}

KeyboardProcess::~KeyboardProcess()
{
    m_process.kill();
}


void KeyboardProcess::start(const QString &executable, const QStringList &arguments)
{
    m_executable = executable;
    m_process.start(m_executable, arguments);
}

bool KeyboardProcess::waitForStarted(int timeout)
{
    return m_process.waitForStarted(timeout);
}

void KeyboardProcess::kill()
{
    m_process.kill();
}

namespace {
static bool isEmptyByteArray(const QByteArray &ba)
{
    return ba.isEmpty();
}
}

void KeyboardProcess::standardOutputReadyRead()
{
    const QByteArray data = m_process.readAllStandardOutput();

    //if we receive multiple commands in one go, just handle the last one
    QList<QByteArray> lines = data.split('\n');
    //remove empty lines
    lines.erase(std::remove_if(lines.begin(), lines.end(), isEmptyByteArray), lines.end());
    const QByteArray line = !lines.isEmpty() ? lines.last() : QByteArray();

    if (line == QByteArrayLiteral("show")) {
        m_windowManager->setX(m_process.pid(), 0);
        m_windowManager->setY(m_process.pid(), 1920-470);
    } else if (line == QByteArrayLiteral("hide")) {
        m_windowManager->setX(m_process.pid(), -10000);
        writeStatusHidden();
    } else {
        qWarning() << "Unexpected output from keyboard process" << line << line.toHex();
    }
}

void KeyboardProcess::standardErrorReadyRead()
{
    const QByteArray data = m_process.readAllStandardError();
    qCritical("[%s|2] %s", qPrintable(m_executable), data.constData());
}

Q_PID KeyboardProcess::pid() const
{
    return m_process.pid();
}

QString KeyboardProcess::errorString() const
{
    return m_process.errorString();
}

void KeyboardProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{

}

void KeyboardProcess::writeStatusHidden()
{
    if (!ppsStatus->setAttribute(QStringLiteral("visible"), QPps::Variant(false)))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();

    if (!ppsStatus->setAttribute(QStringLiteral("size"), QPps::Variant(0)))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();
}
