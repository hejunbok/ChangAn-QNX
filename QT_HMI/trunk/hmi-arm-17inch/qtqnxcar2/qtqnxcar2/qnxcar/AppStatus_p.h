#ifndef QTQNXCAR2_APPSTATUS_P_H
#define QTQNXCAR2_APPSTATUS_P_H

#include "AppStatus.h"

#include <qpps/object.h>

namespace QnxCar {

class AppStatus::Private : public QObject
{
    Q_OBJECT

public:
    Private(AppStatus *qq);

    AppStatus *q;

    QPps::Object *m_ppsObject;

    int m_appstatus;
    int m_carplaystatus;
    int m_mlinkstatus;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
