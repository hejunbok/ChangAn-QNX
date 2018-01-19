#ifndef QTQNXCAR2_HVAC_P_H
#define QTQNXCAR2_HVAC_P_H

#include "Hvac.h"

#include <qpps/object.h>

namespace QnxCar {

class Hvac::Private : public QObject
{
    Q_OBJECT

public:
    Private(Hvac *qq);

    Hvac *q;

    QPps::Object *m_ppsObject;

    int m_airCirculationSetting;
    bool m_airConditioningEnabled;
    int m_fanSettingLeft;
    int m_fanSettingRight;
    int m_fanSpeedLeft;
    int m_fanSpeedRight;
    int m_fanTemperatureLeft;
    int m_fanTemperatureRight;
    int m_heatedSeatLevelLeft;
    int m_heatedSeatLevelRight;
    bool m_rearDefrostEnabled;
    bool m_zoneLinkEnabled;
    bool m_frontWindowHeatEnabled;
    bool m_backWindowHeatEnabled;
//    bool m_outsideCircleEnabled;
//    bool m_insideCircleEnabled;
    bool m_airCondition_AC_modeEnabled;
    bool m_airConditionSwitchOn;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
