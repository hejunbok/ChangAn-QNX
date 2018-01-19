#ifndef QTQNXCAR2_USER_P_H
#define QTQNXCAR2_USER_P_H

#include "User.h"

#include <qpps/object.h>

namespace QnxCar {

class User::Private : public QObject
{
    Q_OBJECT

public:
    Private(User *qq);

    User *q;

    QPps::Object *m_ppsObject;

    int m_id;
    QString m_fullName;
    QString m_avatar;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
