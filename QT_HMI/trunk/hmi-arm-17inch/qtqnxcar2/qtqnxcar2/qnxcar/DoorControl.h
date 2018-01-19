#ifndef QTQNXCAR2_DOOR_CONTROL_H
#define QTQNXCAR2_DOOR_CONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/hvac PPS object
 */
class QTQNXCAR2_EXPORT CarDoorControl : public QObject
{
    Q_OBJECT

    /// The level of the left heated seat in the range from 0 to 3
    Q_PROPERTY(int fl_door_state READ fl_door_state WRITE setFl_door_state NOTIFY fl_door_stateChanged)
    Q_PROPERTY(int fr_door_state READ fr_door_state WRITE setFr_door_state NOTIFY fr_door_stateChanged)
    Q_PROPERTY(int rl_door_state READ rl_door_state WRITE setRl_door_state NOTIFY rl_door_stateChanged)
    Q_PROPERTY(int rr_door_state READ rr_door_state WRITE setRr_door_state NOTIFY rr_door_stateChanged)
    Q_PROPERTY(int bk_door_state READ bk_door_state WRITE setBk_door_state NOTIFY bk_door_stateChanged)
    Q_PROPERTY(int front_light_state READ front_light_state WRITE setFront_light_state NOTIFY front_light_stateChanged)
    Q_PROPERTY(int amfm_state READ amfm_state WRITE setAmfm_state NOTIFY amfm_stateChanged)
    Q_PROPERTY(int amfm_value READ amfm_value WRITE setAmfm_value NOTIFY amfm_valueChanged)
    Q_PROPERTY(int ui_style READ ui_style WRITE setUi_style NOTIFY ui_styleChange)
    Q_PROPERTY(float chairGoBackState READ chairGoBackState WRITE setChairGoBack NOTIFY chairGoBackStateChanged)
    Q_PROPERTY(float chairUpDownState READ chairUpDownState WRITE setChairUpDown NOTIFY chairUpDownStateChanged)
    Q_PROPERTY(float chairRotateState READ chairRotateState WRITE setChairRotate NOTIFY chairRotateStateChanged)
    Q_PROPERTY(int pm_2_5State READ pm_2_5State WRITE setPm_2_5State NOTIFY pm_2_5StateChanged)

public:

    /**
     * Creates a new Hvac object.
     *
     * @param parent The parent object.
     */
    explicit CarDoorControl(QObject *parent = 0);
    int getIndexByDoorControl(QString controlcmd);

    int fl_door_state() const;
    Q_INVOKABLE void setFl_door_state(int fl_door_state);
    int fr_door_state() const;
    Q_INVOKABLE void setFr_door_state(int fr_door_state);
    int rl_door_state() const;
    Q_INVOKABLE void setRl_door_state(int rl_door_state);
    int rr_door_state() const;
    Q_INVOKABLE void setRr_door_state(int rr_door_state);
    int bk_door_state() const;
    Q_INVOKABLE void setBk_door_state(int bk_door_state);
    int front_light_state() const;
    Q_INVOKABLE void setFront_light_state(int front_light_state);
    int amfm_state() const;
    Q_INVOKABLE void setAmfm_state(int amfm_state);
    int amfm_value() const;
    Q_INVOKABLE void setAmfm_value(double amfm_value);
    int ui_style() const;
    Q_INVOKABLE void setUi_style(int ui_style);
    float chairGoBackState(void) const;
    Q_INVOKABLE void setChairGoBack(float value);
    float chairUpDownState(void) const;
    Q_INVOKABLE void setChairUpDown(float value);
    float chairRotateState(void) const;
    Q_INVOKABLE void setChairRotate(float value);
    int pm_2_5State(void) const;
    Q_INVOKABLE void setPm_2_5State(int pm_2_5State);

Q_SIGNALS:
    void fl_door_stateChanged(int);
    void fr_door_stateChanged(int);
    void rl_door_stateChanged(int);
    void rr_door_stateChanged(int);
    void bk_door_stateChanged(int);
    void front_light_stateChanged(int);
    void amfm_stateChanged(int);
    void amfm_valueChanged(int);
    void ui_styleChange(int);
    void chairGoBackStateChanged(float);
    void chairUpDownStateChanged(float);
    void chairRotateStateChanged(float);
    void pm_2_5StateChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
