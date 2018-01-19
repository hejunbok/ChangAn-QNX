#ifndef QTQNXCAR2_SEAT_CONTROL_P_H
#define QTQNXCAR2_SEAT_CONTROL_P_H

#include "SeatControl.h"

#include <qpps/object.h>

namespace QnxCar {

class SeatControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(SeatControl *qq);

    SeatControl *q;

    QPps::Object *m_ppsObject;

    int m_main_heatingstatus;
    int m_main_height_movement;
    int m_main_lumbar_adjustment;
    int m_main_recliner_movement;
    int m_main_slide_movement;
    int m_main_tilt_movement;
    int m_copilot_heatingstatus;
    int m_copilot_height_movement;
    int m_copilot_lumbar_adjustment;
    int m_copilot_recliner_movement;
    int m_copilot_slide_movement;
    int m_copilot_tilt_movement;
    int m_main_massage_state;
    int m_main_massage_level;
    int m_main_ventilation_state;
    int m_main_ventilation_level;
    int m_copilot_massage_state;
    int m_copilot_massage_level;
    int m_copilot_ventilation_state;
    int m_copilot_ventilation_level;

    QString m_watch_type;
    int m_main_store_recall_position_cmd;
    int m_main_slider_movement_status;
    int m_main_recliner_movement_status;
    int m_main_tilt_movement_status;
    int m_main_height_movement_status;
    int m_main_lumbar_movement_status;
    int m_main_slider_position;
    int m_main_recliner_position;
    int m_main_tilt_position;
    int m_main_height_position;
    int m_main_heighting_status;
    int m_main_ventilation_status;

    int m_copilot_store_recall_position_cmd;
    int m_copilot_slider_movement_status;
    int m_copilot_recliner_movement_status;
    int m_copilot_tilt_movement_status;
    int m_copilot_height_movement_status;
    int m_copilot_lumbar_movement_status;
    int m_copilot_slider_position;
    int m_copilot_recliner_position;
    int m_copilot_tilt_position;
    int m_copilot_height_position;
    int m_copilot_heighting_status;
    int m_copilot_ventilation_status;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
