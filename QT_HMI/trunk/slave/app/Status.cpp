#include "Status.h"

#include <QDateTime>
#include <QTimer>

Status::Status(QObject *parent)
    : QObject(parent)
{
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000 * 30); // every 30 seconds
    connect(timer, SIGNAL(timeout()), SLOT(updateTime()));
    timer->start();

    updateTime();
}

void Status::updateTime()
{
    const QString dateTime = QDateTime::currentDateTime().toString(QStringLiteral("h:mm AP"));
    if (dateTime == m_dateTime)
        return;

    m_dateTime = dateTime;
    emit dateTimeChanged();
}

QString Status::dateTime() const
{
    return m_dateTime;
}

bool Status::wifiActive() const
{
    //TODO: read from where?
    return false;
}

bool Status::deviceActive() const
{
    //TODO: read from where?
    return false;
}
