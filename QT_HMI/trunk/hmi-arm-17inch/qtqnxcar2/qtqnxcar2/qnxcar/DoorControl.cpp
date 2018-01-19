#include "DoorControl.h"
#include "DoorControl_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_flControlCmd = "fl";
static const char* pps_frControlCmd = "fr";
static const char* pps_rlControlCmd = "rl";
static const char* pps_rrControlCmd = "rr";
static const char* pps_bkControlCmd = "backdoor";
static const char* pps_frontLightControlCmd = "lights_state";
static const char* pps_amfmStateCmd = "radio_amfm";
static const char* pps_amfmValueCmd = "radio_amfm_value";
static const char* pps_ui_styleCmd = "ui_style";
static const char* pps_chairGoBackCmd = "chair_goback";
static const char* pps_chairUpDownCmd = "chair_updown";
static const char* pps_chairRotateCmd = "chair_rotate";
static const char* pps_pm_2_5_StateCmd = "pm2_5_state";

QString AMFM_CONTROL[2] = {"am", "fm"};

CarDoorControl::Private::Private(CarDoorControl *qq)
    : QObject(qq)
    , q(qq)
    , m_fl(0)
    , m_fr(0)
    , m_rl(0)
    , m_rr(0)
    , m_bk(0)
    , m_front_light(0)
    , m_am_fm(1)
    , m_am_fm_value(0.0)
    , m_ui_style(2)
    , m_chair_go_back(0.0)
    , m_chair_up_down(0.0)
    , m_chair_rotate(0.0)
    , m_pm_2_5_state(1)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/ui"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_fl = q->getIndexByDoorControl(m_ppsObject->attribute(QString::fromLatin1(pps_flControlCmd)).toString());
        m_fr = q->getIndexByDoorControl(m_ppsObject->attribute(QString::fromLatin1(pps_frControlCmd)).toString());
        m_rl = q->getIndexByDoorControl(m_ppsObject->attribute(QString::fromLatin1(pps_rlControlCmd)).toString());
        m_rr = q->getIndexByDoorControl(m_ppsObject->attribute(QString::fromLatin1(pps_rrControlCmd)).toString());
        m_bk = q->getIndexByDoorControl(m_ppsObject->attribute(QString::fromLatin1(pps_bkControlCmd)).toString());
        m_front_light = q->getIndexByDoorControl(m_ppsObject->attribute(QString::fromLatin1(pps_frontLightControlCmd)).toString());
        m_am_fm = (m_ppsObject->attribute(QString::fromLatin1(pps_frontLightControlCmd)).toString() == "am") ? 0 : 1;
        m_am_fm_value = m_ppsObject->attribute(QString::fromLatin1(pps_frontLightControlCmd)).toString().toDouble();
        m_ui_style = m_ppsObject->attribute(QString::fromLatin1(pps_frontLightControlCmd)).toString().toInt();
        m_chair_go_back = m_ppsObject->attribute(QString::fromLatin1(pps_chairGoBackCmd)).toString().toFloat();
        m_chair_up_down = m_ppsObject->attribute(QString::fromLatin1(pps_chairUpDownCmd)).toString().toFloat();
        m_chair_rotate = m_ppsObject->attribute(QString::fromLatin1(pps_chairRotateCmd)).toString().toFloat();
        m_pm_2_5_state = m_ppsObject->attribute(QString::fromLatin1(pps_pm_2_5_StateCmd)).toString().toFloat();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void CarDoorControl::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_flControlCmd)) {
        if (m_fl != attribute.toInt()) {
            m_fl = attribute.toInt();
            emit q->fl_door_stateChanged(m_fl);
        }
    }
    else if (name == QString::fromLatin1(pps_frControlCmd)) {
        if (m_fr != attribute.toInt()) {
            m_fr = attribute.toInt();
            emit q->fr_door_stateChanged(m_fr);
        }
    }
    else if (name == QString::fromLatin1(pps_rlControlCmd)) {
        if (m_rl != attribute.toInt()) {
            m_rl = attribute.toInt();
            emit q->rl_door_stateChanged(m_rl);
        }
    }
    else if (name == QString::fromLatin1(pps_rrControlCmd)) {
        if (m_rr != attribute.toInt()) {
            m_rr = attribute.toInt();
            emit q->rr_door_stateChanged(m_rr);
        }
    }
    else if (name == QString::fromLatin1(pps_bkControlCmd)) {
        if (m_bk != attribute.toInt()) {
            m_bk = attribute.toInt();
            emit q->bk_door_stateChanged(m_bk);
        }
    }
    else if (name == QString::fromLatin1(pps_frontLightControlCmd)) {
        if (m_front_light != attribute.toInt()) {
            m_front_light = attribute.toInt();
            emit q->front_light_stateChanged(m_front_light);
        }
    }
    else if (name == QString::fromLatin1(pps_amfmStateCmd)) {
        if (m_am_fm != (attribute.toString() == "am" ? 0 : 1)) {
            m_am_fm = (attribute.toString() == "am" ? 0 : 1);
            emit q->amfm_stateChanged(m_am_fm);
        }
    }
    else if (name == QString::fromLatin1(pps_amfmValueCmd)) {
        if (m_am_fm_value != attribute.toDouble()) {
            m_am_fm_value = attribute.toDouble();
            emit q->amfm_valueChanged(m_am_fm_value);
        }
    }
    else if (name == QString::fromLatin1(pps_ui_styleCmd)) {
        if (m_ui_style != attribute.toInt()) {
            m_ui_style = attribute.toInt();
            emit q->amfm_valueChanged(m_am_fm_value);
        }
    }
    else if (name == QString::fromLatin1(pps_chairGoBackCmd)) {
        if (m_chair_go_back != attribute.toString().toFloat()) {
            m_chair_go_back = attribute.toString().toFloat();
            emit q->chairGoBackStateChanged(m_am_fm_value);
        }
    }
    else if (name == QString::fromLatin1(pps_chairUpDownCmd)) {
        if (m_chair_up_down != attribute.toString().toFloat()) {
            m_chair_up_down = attribute.toString().toFloat();
            emit q->chairUpDownStateChanged(m_chair_up_down);
        }
    }
    else if (name == QString::fromLatin1(pps_chairGoBackCmd)) {
        if (m_chair_rotate != attribute.toString().toFloat()) {
            m_chair_rotate = attribute.toString().toFloat();
            emit q->chairRotateStateChanged(m_chair_rotate);
        }
    }
    else if (name == QString::fromLatin1(pps_pm_2_5_StateCmd)) {
        if (m_pm_2_5_state != attribute.toString().toInt()) {
            m_pm_2_5_state = attribute.toString().toInt();
            emit q->pm_2_5StateChanged(m_pm_2_5_state);
        }
    }
}

CarDoorControl::CarDoorControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}


int CarDoorControl::getIndexByDoorControl(QString controlcmd)
{
    return controlcmd.toInt();
}

int CarDoorControl::fl_door_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_flControlCmd)).toString();
    d->m_fl = ppsValue.toInt();

    return d->m_fl;
}

void CarDoorControl::setFl_door_state(int fl_door_state)
{
    if (d->m_fl == fl_door_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_flControlCmd), QPps::Variant(fl_door_state))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_flControlCmd);
        return;
    }

    d->m_fl = fl_door_state;
    emit fl_door_stateChanged(d->m_fl);
}

int CarDoorControl::fr_door_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_frControlCmd)).toString();
    d->m_fr = ppsValue.toInt();

    return d->m_fr;
}

void CarDoorControl::setFr_door_state(int fr_door_state)
{
    if (d->m_fr == fr_door_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_frControlCmd), QPps::Variant(fr_door_state))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_frControlCmd);
        return;
    }

    d->m_fr = fr_door_state;
    emit fr_door_stateChanged(d->m_fr);
}

int CarDoorControl::rl_door_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_rlControlCmd)).toString();
    d->m_rl = ppsValue.toInt();

    return d->m_rl;
}

void CarDoorControl::setRl_door_state(int rl_door_state)
{
    if (d->m_rl == rl_door_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_rlControlCmd), QPps::Variant(rl_door_state))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_rlControlCmd);
        return;
    }

    d->m_rl = rl_door_state;
    emit rl_door_stateChanged(d->m_rl);
}

int CarDoorControl::rr_door_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_rrControlCmd)).toString();
    d->m_rr = ppsValue.toInt();

    return d->m_rr;
}

void CarDoorControl::setRr_door_state(int rr_door_state)
{
    if (d->m_rr == rr_door_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_rrControlCmd), QPps::Variant(rr_door_state))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_rrControlCmd);
        return;
    }

    d->m_rr = rr_door_state;
    emit rr_door_stateChanged(d->m_rr);
}

int CarDoorControl::bk_door_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_bkControlCmd)).toString();
    d->m_bk = ppsValue.toInt();

    return d->m_bk;
}

void CarDoorControl::setBk_door_state(int bk_door_state)
{
    if (d->m_bk == bk_door_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_bkControlCmd), QPps::Variant(bk_door_state))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_bkControlCmd);
        return;
    }

    d->m_bk = bk_door_state;
    emit bk_door_stateChanged(d->m_bk);
}

int CarDoorControl::front_light_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_frontLightControlCmd)).toString();
    d->m_front_light = ppsValue.toInt();

    return d->m_front_light;
}
void CarDoorControl::setFront_light_state(int front_light_state)
{
    if (d->m_front_light == front_light_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_frontLightControlCmd), QPps::Variant(front_light_state))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_frontLightControlCmd);
        return;
    }

    d->m_front_light = front_light_state;
    emit front_light_stateChanged(d->m_front_light);
}

int CarDoorControl::amfm_state() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_amfmStateCmd)).toString();
    if(ppsValue == "am")
    {
        d->m_am_fm = 0;
    }
    else{
        d->m_am_fm = 1;
    }

    return d->m_am_fm;
}

void CarDoorControl::setAmfm_state(int amfm_state)
{
    if (d->m_am_fm == amfm_state)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_amfmStateCmd), QPps::Variant(AMFM_CONTROL[amfm_state]))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_amfmStateCmd);
        return;
    }

    d->m_am_fm = amfm_state;
    emit amfm_stateChanged(d->m_am_fm);
}

int CarDoorControl::amfm_value() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_amfmValueCmd)).toString();
    d->m_am_fm_value = ppsValue.toDouble();

    return d->m_am_fm_value;
}

void CarDoorControl::setAmfm_value(double amfm_value)
{
    if (d->m_am_fm_value == amfm_value)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_amfmValueCmd), QPps::Variant(QString::number(amfm_value)))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_amfmValueCmd);
        return;
    }

    d->m_am_fm_value = amfm_value;
    emit amfm_valueChanged(d->m_am_fm_value);
}

int CarDoorControl::ui_style() const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_ui_styleCmd)).toString();
    d->m_ui_style = ppsValue.toInt();

    return d->m_ui_style;
}

void CarDoorControl::setUi_style(int ui_style)
{
    if (d->m_ui_style == ui_style)
        return;

    if(ui_style == 1)
    {
        system("echo ui_style::1 >> /pps/ui &");
    }
    else if(ui_style == 2)
    {
        system("echo ui_style::2 >> /pps/ui &");
    }
    else if(ui_style == 3)
    {
        system("echo ui_style::3 >> /pps/ui &");
    }

    d->m_ui_style = ui_style;
    emit ui_styleChange(d->m_ui_style);
}


float CarDoorControl::chairGoBackState(void) const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_chairGoBackCmd)).toString();
    d->m_chair_go_back = ppsValue.toFloat();

    return d->m_chair_go_back;
}

void CarDoorControl::setChairGoBack(float value)
{
    if (d->m_chair_go_back == value)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_chairGoBackCmd), QPps::Variant(QString::number(value)))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_chairGoBackCmd);
        return;
    }

    d->m_chair_go_back = value;
    emit chairGoBackStateChanged(d->m_chair_go_back);
}

float CarDoorControl::chairUpDownState(void) const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_chairUpDownCmd)).toString();
    d->m_chair_up_down = ppsValue.toFloat();

    return d->m_chair_up_down;
}

void CarDoorControl::setChairUpDown(float value)
{
    if (d->m_chair_up_down == value)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_chairUpDownCmd), QPps::Variant(QString::number(value)))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_chairUpDownCmd);
        return;
    }

    d->m_chair_up_down = value;
    emit chairUpDownStateChanged(d->m_chair_up_down);
}

float CarDoorControl::chairRotateState(void) const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_chairRotateCmd)).toString();
    d->m_chair_rotate = ppsValue.toFloat();

    return d->m_chair_rotate;
}

void CarDoorControl::setChairRotate(float value)
{
    if (d->m_chair_rotate == value)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_chairRotateCmd), QPps::Variant(QString::number(value)))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_chairRotateCmd);
        return;
    }

    d->m_chair_rotate = value;
    emit chairRotateStateChanged(d->m_chair_rotate);
}

int CarDoorControl::pm_2_5State(void) const
{
    QString ppsValue = d->m_ppsObject->attribute(QString::fromLatin1(pps_pm_2_5_StateCmd)).toString();
    d->m_pm_2_5_state = ppsValue.toInt();

    return d->m_pm_2_5_state;
}

void CarDoorControl::setPm_2_5State(int pm_2_5State)
{
    if (d->m_pm_2_5_state == pm_2_5State)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_pm_2_5_StateCmd), QPps::Variant(QString::number(pm_2_5State)))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_pm_2_5_StateCmd);
        return;
    }

    d->m_pm_2_5_state = pm_2_5State;
    emit pm_2_5StateChanged(d->m_pm_2_5_state);
}

}
