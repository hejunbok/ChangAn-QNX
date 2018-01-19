#ifndef QTQNXCAR2_DOOR_CONTROL_P_H
#define QTQNXCAR2_DOOR_CONTROL_P_H

#include "DoorControl.h"

#include <qpps/object.h>

namespace QnxCar {

class CarDoorControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(CarDoorControl *qq);

    CarDoorControl *q;

    QPps::Object *m_ppsObject;

    int m_fl;
    int m_fr;
    int m_rl;
    int m_rr;
    int m_bk;
    int m_front_light;
    int m_am_fm;
    double m_am_fm_value;
    int m_ui_style;
    float m_chair_go_back;
    float m_chair_up_down;
    float m_chair_rotate;
    int m_pm_2_5_state;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
