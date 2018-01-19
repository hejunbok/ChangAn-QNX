#ifndef QTQNXCAR2_GEOLOCATION_P_H
#define QTQNXCAR2_GEOLOCATION_P_H

#include "Geolocation.h"

#include <qpps/object.h>

namespace QnxCar {

class Geolocation::Private : public QObject
{
    Q_OBJECT

public:
    Private(Geolocation *qq);

    Geolocation *q;

    QPps::Object *m_ppsObject;
    QPps::Object *m_vr_control_ppsObject;

    int m_turn;
    int m_cmd;


private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
