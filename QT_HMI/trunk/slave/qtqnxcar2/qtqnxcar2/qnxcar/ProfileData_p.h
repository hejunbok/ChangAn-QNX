#ifndef PROFILEDATA_P_H
#define PROFILEDATA_P_H

#include <QList>
#include <QVariant>

class QSqlRecord;

namespace QnxCar {

struct ProfileData
{
    QString fullName;
    QString deviceId;
    QString theme;
    QString avatar;
    QString avatarFilePath;

    QList<QVariant> values() const;
    static ProfileData fromSqlRecord(const QSqlRecord &record);

    inline bool isValid() const { return !avatar.isEmpty(); }
};

}

#endif // PROFILEDATA_P_H
