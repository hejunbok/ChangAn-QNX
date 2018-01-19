#include "DateTimeFormatting.h"

#include <QDate>
#include <QDateTime>
#include <QString>
#include <QTime>

QString QnxCar::formatDate(const QDate &date)
{
    return date.toString(QLatin1String("MM/dd/yyyy"));
}

QString QnxCar::formatTime(const QTime &time)
{
    return time.toString(QLatin1String("h:mmap"));
}

QString QnxCar::formatDateTime(const QDateTime &dateTime)
{
    return dateTime.toString(QLatin1String("MM/dd/yyyy h:mmap"));
}
