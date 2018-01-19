#ifndef QNXCAR2_DATETIMEFORMATTING_H
#define QNXCAR2_DATETIMEFORMATTING_H

#include "qtqnxcar2_export.h"

class QDate;
class QDateTime;
class QString;
class QTime;

/**
 * Date formatting functions, as replacements for Qt.formatDate() etc.
 * Workaround for QTBUG-35693
 */
namespace QnxCar
{
    QTQNXCAR2_EXPORT QString formatDate(const QDate &date);
    QTQNXCAR2_EXPORT QString formatTime(const QTime &time);
    QTQNXCAR2_EXPORT QString formatDateTime(const QDateTime &dateTime);
}

#endif
