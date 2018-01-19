#ifndef QTQNXCAR2_SEAT_CONTROL_H
#define QTQNXCAR2_SEAT_CONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/hvac PPS object
 */
class QTQNXCAR2_EXPORT SeatControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int main_heatingstatus READ main_heatingstatus WRITE setMain_heatingstatus NOTIFY main_heatingstatusChanged)
    Q_PROPERTY(int main_height_movement READ main_height_movement WRITE setMain_height_movement NOTIFY main_height_movementChanged)
    Q_PROPERTY(int main_lumbar_adjustment READ main_lumbar_adjustment WRITE setMain_lumbar_adjustment NOTIFY main_lumbar_adjustmentChanged)
    Q_PROPERTY(int main_recliner_movement READ main_recliner_movement WRITE setMain_recliner_movement NOTIFY main_recliner_movementChanged)
    Q_PROPERTY(int main_slide_movement READ main_slide_movement WRITE setMain_slide_movement NOTIFY main_slide_movementChanged)
    Q_PROPERTY(int main_tilt_movement READ main_tilt_movement WRITE setMain_tilt_movement NOTIFY main_tilt_movementChanged)

    Q_PROPERTY(int copilot_heatingstatus READ copilot_heatingstatus WRITE setCopilot_heatingstatus NOTIFY copilot_heatingstatusChanged)
    Q_PROPERTY(int copilot_height_movement READ copilot_height_movement WRITE setCopilot_height_movement NOTIFY copilot_height_movementChanged)
    Q_PROPERTY(int copilot_lumbar_adjustment READ copilot_lumbar_adjustment WRITE setCopilot_lumbar_adjustment NOTIFY copilot_lumbar_adjustmentChanged)
    Q_PROPERTY(int copilot_recliner_movement READ copilot_recliner_movement WRITE setCopilot_recliner_movement NOTIFY copilot_recliner_movementChanged)
    Q_PROPERTY(int copilot_slide_movement READ copilot_slide_movement WRITE setCopilot_slide_movement NOTIFY copilot_slide_movementChanged)
    Q_PROPERTY(int copilot_tilt_movement READ copilot_tilt_movement WRITE setCopilot_tilt_movement NOTIFY copilot_tilt_movementChanged)

    Q_PROPERTY(int main_massageState READ main_massageState WRITE main_setMassageState NOTIFY main_massageStateChanged)
    Q_PROPERTY(int main_ventilationState READ main_ventilationState WRITE main_setVentilationState NOTIFY main_ventilationStateChanged)
    Q_PROPERTY(int copilot_massageState READ copilot_massageState WRITE copilot_setMassageState NOTIFY copilot_massageStateChanged)
    Q_PROPERTY(int copilot_ventilationState READ copilot_ventilationState WRITE copilot_setVentilationState NOTIFY copilot_ventilationStateChanged)
    Q_PROPERTY(int main_massageLevel READ main_massageLevel WRITE main_setMassageLevel NOTIFY main_massageLevelChanged)
    Q_PROPERTY(int main_ventilationLevel READ main_ventilationLevel WRITE main_setVentilationLevel NOTIFY main_ventilationLevelChanged)
    Q_PROPERTY(int copilot_massageLevel READ copilot_massageLevel WRITE copilot_setMassageLevel NOTIFY copilot_massageLevelChanged)
    Q_PROPERTY(int copilot_ventilationLevel READ copilot_ventilationLevel WRITE copilot_setVentilationLevel NOTIFY copilot_ventilationLevelChanged)


    Q_PROPERTY(QString watchType READ watchType WRITE setWatchType NOTIFY watchTypeChanged)
    Q_PROPERTY(int mainStoreRecallCmd READ mainStoreRecallCmd WRITE setMainStoreRecallCmd NOTIFY mainStoreRecallCmdChanged)
    Q_PROPERTY(int copilotStoreRecallCmd READ copilotStoreRecallCmd WRITE setCopilotStoreRecallCmd NOTIFY copilotStoreRecallCmdChanged)

    Q_PROPERTY(int main_slider_movement_status READ main_slider_movement_status WRITE main_setSliderMovementStatus NOTIFY main_slider_movement_statusChanged)
    Q_PROPERTY(int main_recliner_movement_status READ main_recliner_movement_status WRITE main_setMainReclinerMovementStatus NOTIFY main_recliner_movement_statusChanged)
    Q_PROPERTY(int main_tilt_movement_status READ main_tilt_movement_status WRITE main_setTiltMovementStatus NOTIFY main_tilt_movement_statusChanged)
    Q_PROPERTY(int main_height_movement_status READ main_height_movement_status WRITE main_setHeightMovementStatus NOTIFY main_height_movement_statusChanged)
    Q_PROPERTY(int main_lumbar_movement_status READ main_lumbar_movement_status WRITE main_setLumbarMovementStatus NOTIFY main_lumbar_movement_statusChanged)
    Q_PROPERTY(int main_slider_position READ main_slider_position WRITE main_setSliderPosition NOTIFY main_slider_positionChanged)
    Q_PROPERTY(int main_recliner_position READ main_recliner_position WRITE main_setReclinerPosition NOTIFY main_recliner_positionChanged)
    Q_PROPERTY(int main_tilt_position READ main_tilt_position WRITE main_setTiltPosition NOTIFY copilot_tilt_positionChanged)
    Q_PROPERTY(int main_height_position READ main_height_position WRITE main_setHeightPosition NOTIFY main_height_positionChanged)
    Q_PROPERTY(int main_heating_status READ main_heating_status WRITE main_setHeatingStatus NOTIFY main_heating_statusChanged)
    Q_PROPERTY(int main_ventilation_status READ main_ventilation_status WRITE main_setVentilationStatus NOTIFY main_ventilation_statusChanged)
    Q_PROPERTY(int copilot_slider_movement_status READ copilot_slider_movement_status WRITE copilot_setSliderMovementStatus NOTIFY copilot_slider_movement_statusChanged)
    Q_PROPERTY(int copilot_recliner_movement_status READ copilot_recliner_movement_status WRITE copilot_setcopilotReclinerMovementStatus NOTIFY copilot_recliner_movement_statusChanged)
    Q_PROPERTY(int copilot_tilt_movement_status READ copilot_tilt_movement_status WRITE copilot_setTiltMovementStatus NOTIFY copilot_tilt_movement_statusChanged)
    Q_PROPERTY(int copilot_height_movement_status READ copilot_height_movement_status WRITE copilot_setHeightMovementStatus NOTIFY copilot_height_movement_statusChanged)
    Q_PROPERTY(int copilot_lumbar_movement_status READ copilot_lumbar_movement_status WRITE copilot_setLumbarMovementStatus NOTIFY copilot_lumbar_movement_statusChanged)
    Q_PROPERTY(int copilot_slider_position READ copilot_slider_position WRITE copilot_setSliderPosition NOTIFY copilot_slider_positionChanged)
    Q_PROPERTY(int copilot_recliner_position READ copilot_recliner_position WRITE copilot_setReclinerPosition NOTIFY copilot_recliner_positionChanged)
    Q_PROPERTY(int copilot_tilt_position READ copilot_tilt_position WRITE copilot_setTiltPosition NOTIFY copilot_tilt_positionChanged)
    Q_PROPERTY(int copilot_height_position READ copilot_height_position WRITE copilot_setHeightPosition NOTIFY copilot_height_positionChanged)
    Q_PROPERTY(int copilot_heating_status READ copilot_heating_status WRITE copilot_setHeatingStatus NOTIFY copilot_heating_statusChanged)
    Q_PROPERTY(int copilot_ventilation_status READ copilot_ventilation_status WRITE copilot_setVentilationStatus NOTIFY copilot_ventilation_statusChanged)

public:
    /**
     * Creates a new Hvac object.
     *
     * @param parent The parent object.
     */
    explicit SeatControl(QObject *parent = 0);

    int main_heatingstatus() const;
    Q_INVOKABLE void setMain_heatingstatus(int main_heatingstatus);
    int main_height_movement() const;
    Q_INVOKABLE void setMain_height_movement(int main_height_movement);
    int main_lumbar_adjustment() const;
    Q_INVOKABLE void setMain_lumbar_adjustment(int main_lumbar_adjustment);
    int main_recliner_movement() const;
    Q_INVOKABLE void setMain_recliner_movement(int main_recliner_movement);
    int main_slide_movement() const;
    Q_INVOKABLE void setMain_slide_movement(int main_slide_movement);
    int main_tilt_movement() const;
    Q_INVOKABLE void setMain_tilt_movement(int main_tilt_movement);

    int copilot_heatingstatus() const;
    Q_INVOKABLE void setCopilot_heatingstatus(int copilot_heatingstatus);
    int copilot_height_movement() const;
    Q_INVOKABLE void setCopilot_height_movement(int copilot_height_movement);
    int copilot_lumbar_adjustment() const;
    Q_INVOKABLE void setCopilot_lumbar_adjustment(int copilot_lumbar_adjustment);
    int copilot_recliner_movement() const;
    Q_INVOKABLE void setCopilot_recliner_movement(int copilot_recliner_movement);
    int copilot_slide_movement() const;
    Q_INVOKABLE void setCopilot_slide_movement(int copilot_slide_movement);
    int copilot_tilt_movement() const;
    Q_INVOKABLE void setCopilot_tilt_movement(int copilot_tilt_movement);

    int main_massageState() const;
    Q_INVOKABLE void  main_setMassageState(int massageState);
    int main_ventilationState() const;
    Q_INVOKABLE void  main_setVentilationState(int ventilationState);
    int copilot_massageState() const;
    Q_INVOKABLE void  copilot_setMassageState(int massageState);
    int copilot_ventilationState() const;
    Q_INVOKABLE void  copilot_setVentilationState(int ventilationState);
    int main_massageLevel() const;
    Q_INVOKABLE void  main_setMassageLevel(int massageLevel);
    int main_ventilationLevel() const;
    Q_INVOKABLE void  main_setVentilationLevel(int ventilationLevel);
    int copilot_massageLevel() const;
    Q_INVOKABLE void  copilot_setMassageLevel(int massageLevel);
    int copilot_ventilationLevel() const;
    Q_INVOKABLE void  copilot_setVentilationLevel(int ventilationLevel);

    QString watchType() const;
    Q_INVOKABLE void setWatchType(QString watchType);
    int mainStoreRecallCmd() const;
    Q_INVOKABLE void setMainStoreRecallCmd(int mainStoreRecallCmd);
    int copilotStoreRecallCmd() const;
    Q_INVOKABLE void setCopilotStoreRecallCmd(int copilotStoreRecallCmd);

    int main_slider_movement_status() const;
    Q_INVOKABLE void  main_setSliderMovementStatus(int main_slider_movement_status);
    int main_recliner_movement_status() const;
    Q_INVOKABLE void  main_setMainReclinerMovementStatus(int main_recliner_movement_status);
    int main_tilt_movement_status() const;
    Q_INVOKABLE void  main_setTiltMovementStatus(int tilt_movement_status);
    int main_height_movement_status() const;
    Q_INVOKABLE void  main_setHeightMovementStatus(int main_height_movement_status);
    int main_lumbar_movement_status() const;
    Q_INVOKABLE void  main_setLumbarMovementStatus(int main_lumbar_movement_status);
    int main_slider_position() const;
    Q_INVOKABLE void  main_setSliderPosition(int main_slider_position);
    int main_recliner_position() const;
    Q_INVOKABLE void  main_setReclinerPosition(int main_recliner_position);
    int main_tilt_position() const;
    Q_INVOKABLE void  main_setTiltPosition(int main_tilt_position);
    int main_height_position() const;
    Q_INVOKABLE void  main_setHeightPosition(int main_height_position);
    int main_heating_status() const;
    Q_INVOKABLE void  main_setHeatingStatus(int main_heating_status);
    int main_ventilation_status() const;
    Q_INVOKABLE void  main_setVentilationStatus(int main_ventilation_status);

    int copilot_slider_movement_status() const;
    Q_INVOKABLE void  copilot_setSliderMovementStatus(int copilot_slider_movement_status);
    int copilot_recliner_movement_status() const;
    Q_INVOKABLE void  copilot_setcopilotReclinerMovementStatus(int copilot_recliner_movement_status);
    int copilot_tilt_movement_status() const;
    Q_INVOKABLE void  copilot_setTiltMovementStatus(int tilt_movement_status);
    int copilot_height_movement_status() const;
    Q_INVOKABLE void  copilot_setHeightMovementStatus(int copilot_height_movement_status);
    int copilot_lumbar_movement_status() const;
    Q_INVOKABLE void  copilot_setLumbarMovementStatus(int copilot_lumbar_movement_status);
    int copilot_slider_position() const;
    Q_INVOKABLE void  copilot_setSliderPosition(int copilot_slider_position);
    int copilot_recliner_position() const;
    Q_INVOKABLE void  copilot_setReclinerPosition(int copilot_recliner_position);
    int copilot_tilt_position() const;
    Q_INVOKABLE void  copilot_setTiltPosition(int copilot_tilt_position);
    int copilot_height_position() const;
    Q_INVOKABLE void  copilot_setHeightPosition(int copilot_height_position);
    int copilot_heating_status() const;
    Q_INVOKABLE void  copilot_setHeatingStatus(int copilot_heating_status);
    int copilot_ventilation_status() const;
    Q_INVOKABLE void  copilot_setVentilationStatus(int copilot_ventilation_status);

Q_SIGNALS:
    void main_heatingstatusChanged(int);
    void main_height_movementChanged(int);
    void main_lumbar_adjustmentChanged(int);
    void main_recliner_movementChanged(int);
    void main_slide_movementChanged(int);
    void main_tilt_movementChanged(int);
    void copilot_heatingstatusChanged(int);
    void copilot_height_movementChanged(int);
    void copilot_lumbar_adjustmentChanged(int);
    void copilot_recliner_movementChanged(int);
    void copilot_slide_movementChanged(int);
    void copilot_tilt_movementChanged(int);

    void main_massageStateChanged(int);
    void main_ventilationStateChanged(int);
    void copilot_massageStateChanged(int);
    void copilot_ventilationStateChanged(int);
    void main_massageLevelChanged(int);
    void main_ventilationLevelChanged(int);
    void copilot_massageLevelChanged(int);
    void copilot_ventilationLevelChanged(int);

    void watchTypeChanged(QString);
    void mainStoreRecallCmdChanged(int);
    void copilotStoreRecallCmdChanged(int);

    int main_slider_movement_statusChanged(int);
    int main_recliner_movement_statusChanged(int);
    int main_tilt_movement_statusChanged(int);
    int main_height_movement_statusChanged(int);
    int main_lumbar_movement_statusChanged(int);
    int main_slider_positionChanged(int);
    int main_recliner_positionChanged(int);
    int main_tilt_positionChanged(int);
    int main_height_positionChanged(int);
    int main_heating_statusChanged(int);
    int main_ventilation_statusChanged(int);

    int copilot_slider_movement_statusChanged(int);
    int copilot_recliner_movement_statusChanged(int);
    int copilot_tilt_movement_statusChanged(int);
    int copilot_height_movement_statusChanged(int);
    int copilot_lumbar_movement_statusChanged(int);
    int copilot_slider_positionChanged(int);
    int copilot_recliner_positionChanged(int);
    int copilot_tilt_positionChanged(int);
    int copilot_height_positionChanged(int);
    int copilot_heating_statusChanged(int);
    int copilot_ventilation_statusChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
