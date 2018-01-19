#ifndef QTQNXCAR2_LAMPSTATUS_P_H
#define QTQNXCAR2_LAMPSTATUS_P_H

#include "LampStatus.h"

#include <qpps/object.h>

namespace QnxCar {

class LampStatus::Private : public QObject
{
    Q_OBJECT

public:
    Private(LampStatus *qq);

    LampStatus *q;

    QPps::Object *m_ppsObject;

    int m_ambience;
    bool m_intersection;
    bool m_turn_left;
    bool m_turn_right;
    int m_lampBrighrness;
    int m_steerwheel_shock;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
