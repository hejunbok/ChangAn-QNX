#include "SeatControl.h"
#include "SeatControl_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_main_heatingstatusKey = "main_heatingstatus";
static const char* pps_main_height_movementKey = "main_height_movement";
static const char* pps_main_lumbar_adjustmentKey = "main_lumbar_adjustment";
static const char* pps_main_recliner_movementKey = "main_recliner_movement";
static const char* pps_main_slide_movementKey = "main_slide_movement";
static const char* pps_main_tilt_movementKey = "main_tilt_movement";
static const char* pps_copilot_heatingstatusKey = "copilot_heatingstatus";
static const char* pps_copilot_height_movementKey = "copilot_height_movement";
static const char* pps_copilot_lumbar_adjustmentKey = "copilot_lumbar_adjustment";
static const char* pps_copilot_recliner_movementKey = "copilot_recliner_movement";
static const char* pps_copilot_slide_movementKey = "copilot_slide_movement";
static const char* pps_copilot_tilt_movementKey = "copilot_tilt_movement";
static const char* pps_main_massage_stateKey = "main_massage";
static const char* pps_main_ventilation_stateKey = "main_ventilation";
static const char* pps_copilot_massage_stateKey = "copilot_massage";
static const char* pps_copilot_ventilation_stateKey = "copilot_ventilation";
static const char* pps_main_massage_levelKey = "main_massage_level";
static const char* pps_main_ventilation_levelKey = "main_ventilation_level";
static const char* pps_copilot_massage_levelKey = "copilot_massage_level";
static const char* pps_copilot_ventilation_levelKey = "copilot_ventilation_level";

static const char* pps_watch_typeKey = "watch_kind";

static const char* pps_main_store_recall_position_cmdKey = "main_store_recall_position_cmd";
static const char* pps_main_slider_movement_statusKey = "main_slider_movement_status";
static const char* pps_main_recliner_movement_statusKey = "main_recliner_movement_status";
static const char* pps_main_tilt_movement_statusKey = "main_tilt_movement_status";
static const char* pps_main_height_movement_statusKey = "main_height_movement_status";
static const char* pps_main_lumbar_movement_statusKey = "main_lumbar_movement_status";
static const char* pps_main_slider_positionKey = "main_slider_position";
static const char* pps_main_recliner_positionKey = "main_recliner_position";
static const char* pps_main_tilt_positionKey = "main_tilt_position";
static const char* pps_main_height_positionKey = "main_height_position";
static const char* pps_main_heighting_statusKey = "main_heighting_status";
static const char* pps_main_ventilation_statusKey = "main_ventilation_status";
static const char* pps_copilot_store_recall_position_cmdKey = "copilot_store_recall_position_cmd";
static const char* pps_copilot_slider_movement_statusKey = "copilot_slider_movement_status";
static const char* pps_copilot_recliner_movement_statusKey = "copilot_recliner_movement_status";
static const char* pps_copilot_tilt_movement_statusKey = "copilot_tilt_movement_status";
static const char* pps_copilot_height_movement_statusKey = "copilot_height_movement_status";
static const char* pps_copilot_lumbar_movement_statusKey = "copilot_lumbar_movement_status";
static const char* pps_copilot_slider_positionKey = "copilot_slider_position";
static const char* pps_copilot_recliner_positionKey = "copilot_recliner_position";
static const char* pps_copilot_tilt_positionKey = "copilot_tilt_position";
static const char* pps_copilot_height_positionKey = "copilot_height_position";
static const char* pps_copilot_heighting_statusKey = "copilot_heighting_status";
static const char* pps_copilot_ventilation_statusKey = "copilot_ventilation_status";

SeatControl::Private::Private(SeatControl *qq)
    : QObject(qq)
    , q(qq)
    , m_main_heatingstatus(0)
    , m_main_height_movement(0)
    , m_main_lumbar_adjustment(0)
    , m_main_recliner_movement(0)
    , m_main_slide_movement(0)
    , m_main_tilt_movement(0)
    , m_copilot_heatingstatus(0)
    , m_copilot_height_movement(0)
    , m_copilot_lumbar_adjustment(0)
    , m_copilot_recliner_movement(0)
    , m_copilot_slide_movement(0)
    , m_copilot_tilt_movement(0)
    , m_main_massage_state(0)
    , m_main_ventilation_state(0)
    , m_copilot_massage_state(0)
    , m_copilot_ventilation_state(0)
    , m_main_massage_level(0)
    , m_main_ventilation_level(0)
    , m_copilot_massage_level(0)
    , m_copilot_ventilation_level(0)
    , m_watch_type(QStringLiteral("none"))
    , m_main_store_recall_position_cmd(0)
    , m_copilot_store_recall_position_cmd(0)
    , m_main_slider_movement_status(0)
    , m_main_recliner_movement_status(0)
    , m_main_tilt_movement_status(0)
    , m_main_height_movement_status(0)
    , m_main_lumbar_movement_status(0)
    , m_main_slider_position(0)
    , m_main_recliner_position(0)
    , m_main_tilt_position(0)
    , m_main_height_position(0)
    , m_main_heighting_status(0)
    , m_main_ventilation_status(0)
    , m_copilot_slider_movement_status(0)
    , m_copilot_recliner_movement_status(0)
    , m_copilot_tilt_movement_status(0)
    , m_copilot_height_movement_status(0)
    , m_copilot_lumbar_movement_status(0)
    , m_copilot_slider_position(0)
    , m_copilot_recliner_position(0)
    , m_copilot_tilt_position(0)
    , m_copilot_height_position(0)
    , m_copilot_heighting_status(0)
    , m_copilot_ventilation_status(0)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/seatcontrol"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_main_heatingstatus = m_ppsObject->attribute(QString::fromLatin1(pps_main_heatingstatusKey)).toInt();
        m_main_height_movement = m_ppsObject->attribute(QString::fromLatin1(pps_main_height_movementKey)).toInt();
        m_main_lumbar_adjustment = m_ppsObject->attribute(QString::fromLatin1(pps_main_lumbar_adjustmentKey)).toInt();
        m_main_recliner_movement = m_ppsObject->attribute(QString::fromLatin1(pps_main_recliner_movementKey)).toInt();
        m_main_slide_movement = m_ppsObject->attribute(QString::fromLatin1(pps_main_slide_movementKey)).toInt();
        m_main_tilt_movement = m_ppsObject->attribute(QString::fromLatin1(pps_main_tilt_movementKey)).toInt();
        m_copilot_heatingstatus = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_heatingstatusKey)).toInt();
        m_copilot_height_movement = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_height_movementKey)).toInt();
        m_copilot_lumbar_adjustment = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_lumbar_adjustmentKey)).toInt();
        m_copilot_recliner_movement = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_recliner_movementKey)).toInt();
        m_copilot_slide_movement = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_slide_movementKey)).toInt();
        m_copilot_tilt_movement = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_tilt_movementKey)).toInt();
        m_main_massage_state = m_ppsObject->attribute(QString::fromLatin1(pps_main_massage_stateKey)).toInt();
        m_main_ventilation_state = m_ppsObject->attribute(QString::fromLatin1(pps_main_ventilation_stateKey)).toInt();
        m_copilot_massage_state = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_massage_stateKey)).toInt();
        m_copilot_ventilation_state = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_ventilation_stateKey)).toInt();
        m_main_massage_level = m_ppsObject->attribute(QString::fromLatin1(pps_main_massage_levelKey)).toInt();
        m_main_ventilation_level = m_ppsObject->attribute(QString::fromLatin1(pps_main_ventilation_levelKey)).toInt();
        m_copilot_massage_level = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_massage_levelKey)).toInt();
        m_copilot_ventilation_level = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_ventilation_levelKey)).toInt();

        m_watch_type = m_ppsObject->attribute(QString::fromLatin1(pps_watch_typeKey)).toString();
        m_main_store_recall_position_cmd = m_ppsObject->attribute(QString::fromLatin1(pps_main_store_recall_position_cmdKey)).toInt();
        m_copilot_store_recall_position_cmd = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_store_recall_position_cmdKey)).toInt();

        m_main_slider_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_slider_movement_statusKey)).toInt();
        m_main_recliner_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_recliner_movement_statusKey)).toInt();
        m_main_tilt_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_tilt_movement_statusKey)).toInt();
        m_main_height_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_height_movement_statusKey)).toInt();
        m_main_lumbar_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_lumbar_movement_statusKey)).toInt();
        m_main_slider_position = m_ppsObject->attribute(QString::fromLatin1(pps_main_slider_positionKey)).toInt();
        m_main_recliner_position = m_ppsObject->attribute(QString::fromLatin1(pps_main_recliner_positionKey)).toInt();
        m_main_tilt_position = m_ppsObject->attribute(QString::fromLatin1(pps_main_tilt_positionKey)).toInt();
        m_main_height_position = m_ppsObject->attribute(QString::fromLatin1(pps_main_height_positionKey)).toInt();
        m_main_heighting_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_heighting_statusKey)).toInt();
        m_main_ventilation_status = m_ppsObject->attribute(QString::fromLatin1(pps_main_ventilation_statusKey)).toInt();

        m_copilot_slider_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_slider_movement_statusKey)).toInt();
        m_copilot_recliner_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_recliner_movement_statusKey)).toInt();
        m_copilot_tilt_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_tilt_movement_statusKey)).toInt();
        m_copilot_height_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_height_movement_statusKey)).toInt();
        m_copilot_lumbar_movement_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_lumbar_movement_statusKey)).toInt();
        m_copilot_slider_position = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_slider_positionKey)).toInt();
        m_copilot_recliner_position = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_recliner_positionKey)).toInt();
        m_copilot_tilt_position = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_tilt_positionKey)).toInt();
        m_copilot_height_position = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_height_positionKey)).toInt();
        m_copilot_heighting_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_heighting_statusKey)).toInt();
        m_copilot_ventilation_status = m_ppsObject->attribute(QString::fromLatin1(pps_copilot_ventilation_statusKey)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void SeatControl::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_main_heatingstatusKey)) {
        if (m_main_heatingstatus != attribute.toInt()) {
            m_main_heatingstatus = attribute.toInt();
            emit q->main_heatingstatusChanged(m_main_heatingstatus);
        }
    } else if (name == QString::fromLatin1(pps_main_height_movementKey)) {
        if (m_main_height_movement != attribute.toInt()) {
            m_main_height_movement = attribute.toInt();
            emit q->main_height_movementChanged(m_main_height_movement);
        }
    } else if (name == QString::fromLatin1(pps_main_lumbar_adjustmentKey)) {
        if (m_main_lumbar_adjustment != attribute.toInt()) {
            m_main_lumbar_adjustment = attribute.toInt();
            emit q->main_lumbar_adjustmentChanged(m_main_lumbar_adjustment);
        }
    } else if (name == QString::fromLatin1(pps_main_recliner_movementKey)) {
        if (m_main_recliner_movement != attribute.toInt()) {
            m_main_recliner_movement = attribute.toInt();
            emit q->main_recliner_movementChanged(m_main_recliner_movement);
        }
    } else if (name == QString::fromLatin1(pps_main_slide_movementKey)) {
        if (m_main_slide_movement != attribute.toInt()) {
            m_main_slide_movement = attribute.toInt();
            emit q->main_slide_movementChanged(m_main_slide_movement);
        }
    } else if (name == QString::fromLatin1(pps_main_tilt_movementKey)) {
        if (m_main_tilt_movement != attribute.toInt()) {
            m_main_tilt_movement = attribute.toInt();
            emit q->main_tilt_movementChanged(m_main_tilt_movement);
        }
    } else if (name == QString::fromLatin1(pps_copilot_heatingstatusKey)) {
        if (m_copilot_heatingstatus != attribute.toInt()) {
            m_copilot_heatingstatus = attribute.toInt();
            emit q->copilot_heatingstatusChanged(m_copilot_heatingstatus);
        }
    } else if (name == QString::fromLatin1(pps_copilot_height_movementKey)) {
        if (m_copilot_height_movement != attribute.toInt()) {
            m_copilot_height_movement = attribute.toInt();
            emit q->copilot_height_movementChanged(m_copilot_height_movement);
        }
    } else if (name == QString::fromLatin1(pps_copilot_lumbar_adjustmentKey)) {
        if (m_copilot_lumbar_adjustment != attribute.toInt()) {
            m_copilot_lumbar_adjustment = attribute.toInt();
            emit q->copilot_lumbar_adjustmentChanged(m_copilot_lumbar_adjustment);
        }
    } else if (name == QString::fromLatin1(pps_copilot_recliner_movementKey)) {
        if (m_copilot_recliner_movement != attribute.toInt()) {
            m_copilot_recliner_movement = attribute.toInt();
            emit q->copilot_recliner_movementChanged(m_copilot_recliner_movement);
        }
    } else if (name == QString::fromLatin1(pps_copilot_slide_movementKey)) {
        if (m_copilot_slide_movement != attribute.toInt()) {
            m_copilot_slide_movement = attribute.toInt();
            emit q->copilot_slide_movementChanged(m_copilot_slide_movement);
        }
    } else if (name == QString::fromLatin1(pps_copilot_tilt_movementKey)) {
        if (m_copilot_tilt_movement != attribute.toInt()) {
            m_copilot_tilt_movement = attribute.toInt();
            emit q->copilot_tilt_movementChanged(m_copilot_tilt_movement);
        }
    } else if (name == QString::fromLatin1(pps_main_massage_stateKey)) {
        if (m_main_massage_state != attribute.toInt()) {
            m_main_massage_state = attribute.toInt();
            emit q->main_massageStateChanged(m_main_massage_state);
        }
    } else if (name == QString::fromLatin1(pps_main_ventilation_stateKey)) {
        if (m_main_ventilation_state != attribute.toInt()) {
            m_main_ventilation_state = attribute.toInt();
            emit q->main_ventilationStateChanged(m_main_ventilation_state);
        }
    } else if (name == QString::fromLatin1(pps_copilot_massage_stateKey)) {
        if (m_copilot_massage_state != attribute.toInt()) {
            m_copilot_massage_state = attribute.toInt();
            emit q->copilot_massageStateChanged(m_copilot_massage_state);
        }
    } else if (name == QString::fromLatin1(pps_copilot_ventilation_stateKey)) {
        if (m_copilot_ventilation_state != attribute.toInt()) {
            m_copilot_ventilation_state = attribute.toInt();
            emit q->copilot_ventilationStateChanged(m_copilot_ventilation_state);
        }
    } else if (name == QString::fromLatin1(pps_main_massage_levelKey)) {
        if (m_main_massage_level != attribute.toInt()) {
            m_main_massage_level = attribute.toInt();
            emit q->main_massageLevelChanged(m_main_massage_level);
        }
    } else if (name == QString::fromLatin1(pps_main_ventilation_levelKey)) {
        if (m_main_ventilation_level != attribute.toInt()) {
            m_main_ventilation_level = attribute.toInt();
            emit q->main_ventilationLevelChanged(m_main_ventilation_level);
        }
    } else if (name == QString::fromLatin1(pps_copilot_massage_levelKey)) {
        if (m_copilot_massage_level != attribute.toInt()) {
            m_copilot_massage_level = attribute.toInt();
            emit q->copilot_massageLevelChanged(m_copilot_massage_level);
        }
    } else if (name == QString::fromLatin1(pps_copilot_ventilation_levelKey)) {
        if (m_copilot_ventilation_level != attribute.toInt()) {
            m_copilot_ventilation_level = attribute.toInt();
            emit q->copilot_ventilationLevelChanged(m_copilot_ventilation_level);
        }
    } else if (name == QString::fromLatin1(pps_watch_typeKey)) {
        if (m_watch_type != attribute.toString()) {
            m_watch_type = attribute.toString();
            emit q->watchTypeChanged(m_watch_type);
        }
    } else if (name == QString::fromLatin1(pps_main_store_recall_position_cmdKey)) {
        if (m_main_store_recall_position_cmd != attribute.toInt()) {
            m_main_store_recall_position_cmd = attribute.toInt();
            emit q->mainStoreRecallCmdChanged(m_main_store_recall_position_cmd);
        }
    } else if (name == QString::fromLatin1(pps_copilot_store_recall_position_cmdKey)) {
        if (m_copilot_store_recall_position_cmd != attribute.toInt()) {
            m_copilot_store_recall_position_cmd = attribute.toInt();
            emit q->copilotStoreRecallCmdChanged(m_copilot_store_recall_position_cmd);
        }
    } else if (name == QString::fromLatin1(pps_main_slider_movement_statusKey)) {
        if (m_main_slider_movement_status != attribute.toInt()) {
            m_main_slider_movement_status = attribute.toInt();
            emit q->main_slider_movement_statusChanged(m_main_slider_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_main_recliner_movement_statusKey)) {
        if (m_main_recliner_movement_status != attribute.toInt()) {
            m_main_recliner_movement_status = attribute.toInt();
            emit q->copilotStoreRecallCmdChanged(m_main_recliner_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_tilt_movement_statusKey)) {
        if (m_main_tilt_movement_status != attribute.toInt()) {
            m_main_tilt_movement_status = attribute.toInt();
            emit q->main_tilt_movement_statusChanged(m_main_tilt_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_main_height_movement_statusKey)) {
        if (m_main_height_movement_status != attribute.toInt()) {
            m_main_height_movement_status = attribute.toInt();
            emit q->main_height_movement_statusChanged(m_main_height_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_main_lumbar_movement_statusKey)) {
        if (m_main_lumbar_movement_status != attribute.toInt()) {
            m_main_lumbar_movement_status = attribute.toInt();
            emit q->main_lumbar_movement_statusChanged(m_main_lumbar_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_main_slider_positionKey)) {
        if (m_main_slider_position != attribute.toInt()) {
            m_main_slider_position = attribute.toInt();
            emit q->main_slider_movement_statusChanged(m_main_slider_position);
        }
    } else if (name == QString::fromLatin1(pps_main_recliner_positionKey)) {
        if (m_main_recliner_position != attribute.toInt()) {
            m_main_recliner_position = attribute.toInt();
            emit q->main_recliner_positionChanged(m_main_recliner_position);
        }
    } else if (name == QString::fromLatin1(pps_main_tilt_positionKey)) {
        if (m_main_tilt_position != attribute.toInt()) {
            m_main_tilt_position = attribute.toInt();
            emit q->main_tilt_positionChanged(m_main_tilt_position);
        }
    } else if (name == QString::fromLatin1(pps_main_height_positionKey)) {
        if (m_main_height_position != attribute.toInt()) {
            m_main_height_position = attribute.toInt();
            emit q->main_height_positionChanged(m_main_height_position);
        }
    } else if (name == QString::fromLatin1(pps_main_heatingstatusKey)) {
        if (m_main_heatingstatus != attribute.toInt()) {
            m_main_heatingstatus = attribute.toInt();
            emit q->main_heating_statusChanged(m_main_heatingstatus);
        }
    } else if (name == QString::fromLatin1(pps_main_ventilation_statusKey)) {
        if (m_main_ventilation_status != attribute.toInt()) {
            m_main_ventilation_status = attribute.toInt();
            emit q->main_ventilation_statusChanged(m_main_ventilation_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_slider_movement_statusKey)) {
        if (m_copilot_slider_movement_status != attribute.toInt()) {
            m_copilot_slider_movement_status = attribute.toInt();
            emit q->copilot_slider_movement_statusChanged(m_copilot_slider_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_recliner_movement_statusKey)) {
        if (m_copilot_recliner_movement_status != attribute.toInt()) {
            m_copilot_recliner_movement_status = attribute.toInt();
            emit q->copilotStoreRecallCmdChanged(m_copilot_recliner_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_tilt_movement_statusKey)) {
        if (m_copilot_tilt_movement_status != attribute.toInt()) {
            m_copilot_tilt_movement_status = attribute.toInt();
            emit q->copilot_tilt_movement_statusChanged(m_copilot_tilt_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_height_movement_statusKey)) {
        if (m_copilot_height_movement_status != attribute.toInt()) {
            m_copilot_height_movement_status = attribute.toInt();
            emit q->copilot_height_movement_statusChanged(m_copilot_height_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_lumbar_movement_statusKey)) {
        if (m_copilot_lumbar_movement_status != attribute.toInt()) {
            m_copilot_lumbar_movement_status = attribute.toInt();
            emit q->copilot_lumbar_movement_statusChanged(m_copilot_lumbar_movement_status);
        }
    } else if (name == QString::fromLatin1(pps_copilot_slider_positionKey)) {
        if (m_copilot_slider_position != attribute.toInt()) {
            m_copilot_slider_position = attribute.toInt();
            emit q->copilot_slider_movement_statusChanged(m_copilot_slider_position);
        }
    } else if (name == QString::fromLatin1(pps_copilot_recliner_positionKey)) {
        if (m_copilot_recliner_position != attribute.toInt()) {
            m_copilot_recliner_position = attribute.toInt();
            emit q->copilot_recliner_positionChanged(m_copilot_recliner_position);
        }
    } else if (name == QString::fromLatin1(pps_copilot_tilt_positionKey)) {
        if (m_copilot_tilt_position != attribute.toInt()) {
            m_copilot_tilt_position = attribute.toInt();
            emit q->copilot_tilt_positionChanged(m_copilot_tilt_position);
        }
    } else if (name == QString::fromLatin1(pps_copilot_height_positionKey)) {
        if (m_copilot_height_position != attribute.toInt()) {
            m_copilot_height_position = attribute.toInt();
            emit q->copilot_height_positionChanged(m_copilot_height_position);
        }
    } else if (name == QString::fromLatin1(pps_copilot_heatingstatusKey)) {
        if (m_copilot_heatingstatus != attribute.toInt()) {
            m_copilot_heatingstatus = attribute.toInt();
            emit q->copilot_heating_statusChanged(m_copilot_heatingstatus);
        }
    } else if (name == QString::fromLatin1(pps_copilot_ventilation_statusKey)) {
        if (m_copilot_ventilation_status != attribute.toInt()) {
            m_copilot_ventilation_status = attribute.toInt();
            emit q->copilot_ventilation_statusChanged(m_copilot_ventilation_status);
        }
    }
}

SeatControl::SeatControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int SeatControl::main_heatingstatus() const
{
    return d->m_main_heatingstatus;
}

void SeatControl::setMain_heatingstatus(int main_heatingstatus)
{
    if (d->m_main_heatingstatus == main_heatingstatus)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_heatingstatusKey), QPps::Variant(main_heatingstatus))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_heatingstatusKey);
        return;
    }

    d->m_main_heatingstatus = main_heatingstatus;
    emit main_heatingstatusChanged(d->m_main_heatingstatus);
}

int SeatControl::main_height_movement() const
{
    return d->m_main_height_movement;
}
void SeatControl::setMain_height_movement(int main_height_movement)
{
    if (d->m_main_height_movement == main_height_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_height_movementKey), QPps::Variant(main_height_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_height_movementKey);
        return;
    }

    d->m_main_height_movement = main_height_movement;
    emit main_height_movementChanged(d->m_main_height_movement);
}

int SeatControl::main_lumbar_adjustment() const
{
    return d->m_main_lumbar_adjustment;
}

void SeatControl::setMain_lumbar_adjustment(int main_lumbar_adjustment)
{
    if (d->m_main_lumbar_adjustment == main_lumbar_adjustment)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_lumbar_adjustmentKey), QPps::Variant(main_lumbar_adjustment))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_lumbar_adjustmentKey);
        return;
    }

    d->m_main_lumbar_adjustment = main_lumbar_adjustment;
    emit main_lumbar_adjustmentChanged(d->m_main_lumbar_adjustment);
}

int SeatControl::main_recliner_movement() const
{
    return d->m_main_recliner_movement;
}

void SeatControl::setMain_recliner_movement(int main_recliner_movement)
{
    if (d->m_main_recliner_movement == main_recliner_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_recliner_movementKey), QPps::Variant(main_recliner_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_recliner_movementKey);
        return;
    }

    d->m_main_recliner_movement = main_recliner_movement;
    emit main_recliner_movementChanged(d->m_main_recliner_movement);
}

int SeatControl::main_slide_movement() const
{
    return d->m_main_slide_movement;
}

void SeatControl::setMain_slide_movement(int main_slide_movement)
{
    if (d->m_main_slide_movement == main_slide_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_slide_movementKey), QPps::Variant(main_slide_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_slide_movementKey);
        return;
    }

    d->m_main_slide_movement = main_slide_movement;
    emit main_slide_movementChanged(d->m_main_slide_movement);
}

int SeatControl::main_tilt_movement() const
{
    return d->m_main_tilt_movement;
}

void SeatControl::setMain_tilt_movement(int main_tilt_movement)
{
    if (d->m_main_tilt_movement == main_tilt_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_tilt_movementKey), QPps::Variant(main_tilt_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_tilt_movementKey);
        return;
    }

    d->m_main_tilt_movement = main_tilt_movement;
    emit main_tilt_movementChanged(d->m_main_tilt_movement);
}

int SeatControl::copilot_heatingstatus() const
{
    return d->m_copilot_heatingstatus;
}

void SeatControl::setCopilot_heatingstatus(int copilot_heatingstatus)
{
    if (d->m_copilot_heatingstatus == copilot_heatingstatus)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_heatingstatusKey), QPps::Variant(copilot_heatingstatus))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_heatingstatusKey);
        return;
    }

    d->m_copilot_heatingstatus = copilot_heatingstatus;
    emit copilot_heatingstatusChanged(d->m_copilot_heatingstatus);
}

int SeatControl::copilot_height_movement() const
{
    return d->m_copilot_height_movement;
}
void SeatControl::setCopilot_height_movement(int copilot_height_movement)
{
    if (d->m_copilot_height_movement == copilot_height_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_height_movementKey), QPps::Variant(copilot_height_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_height_movementKey);
        return;
    }

    d->m_copilot_height_movement = copilot_height_movement;
    emit copilot_height_movementChanged(d->m_copilot_height_movement);
}

int SeatControl::copilot_lumbar_adjustment() const
{
    return d->m_copilot_lumbar_adjustment;
}

void SeatControl::setCopilot_lumbar_adjustment(int copilot_lumbar_adjustment)
{
    if (d->m_copilot_lumbar_adjustment == copilot_lumbar_adjustment)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_lumbar_adjustmentKey), QPps::Variant(copilot_lumbar_adjustment))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_lumbar_adjustmentKey);
        return;
    }

    d->m_copilot_lumbar_adjustment = copilot_lumbar_adjustment;
    emit copilot_lumbar_adjustmentChanged(d->m_copilot_lumbar_adjustment);
}

int SeatControl::copilot_recliner_movement() const
{
    return d->m_copilot_recliner_movement;
}

void SeatControl::setCopilot_recliner_movement(int copilot_recliner_movement)
{
    if (d->m_copilot_recliner_movement == copilot_recliner_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_recliner_movementKey), QPps::Variant(copilot_recliner_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_recliner_movementKey);
        return;
    }

    d->m_copilot_recliner_movement = copilot_recliner_movement;
    emit copilot_recliner_movementChanged(d->m_copilot_recliner_movement);
}

int SeatControl::copilot_slide_movement() const
{
    return d->m_copilot_slide_movement;
}

void SeatControl::setCopilot_slide_movement(int copilot_slide_movement)
{
    if (d->m_copilot_slide_movement == copilot_slide_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_slide_movementKey), QPps::Variant(copilot_slide_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_slide_movementKey);
        return;
    }

    d->m_copilot_slide_movement = copilot_slide_movement;
    emit copilot_slide_movementChanged(d->m_copilot_slide_movement);
}

int SeatControl::copilot_tilt_movement() const
{
    return d->m_copilot_tilt_movement;
}

void SeatControl::setCopilot_tilt_movement(int copilot_tilt_movement)
{
    if (d->m_copilot_tilt_movement == copilot_tilt_movement)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_tilt_movementKey), QPps::Variant(copilot_tilt_movement))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_tilt_movementKey);
        return;
    }

    d->m_copilot_tilt_movement = copilot_tilt_movement;
    emit copilot_tilt_movementChanged(d->m_copilot_tilt_movement);
}

int SeatControl::main_massageState() const
{
    return d->m_main_massage_state;
}

void  SeatControl::main_setMassageState(int massageState)
{
    if (d->m_main_massage_state == massageState)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_massage_stateKey), QPps::Variant(massageState))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_massage_stateKey);
        return;
    }

    d->m_main_massage_state = massageState;
    emit main_massageStateChanged(d->m_main_massage_state);
}

int SeatControl::main_ventilationState() const
{
    return d->m_main_ventilation_state;
}

void  SeatControl::main_setVentilationState(int ventilationState)
{
    if (d->m_main_ventilation_state == ventilationState)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_ventilation_stateKey), QPps::Variant(ventilationState))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_ventilation_stateKey);
        return;
    }

    d->m_main_ventilation_state = ventilationState;
    emit main_ventilationStateChanged(d->m_main_ventilation_state);
}

int SeatControl::copilot_massageState() const
{
    return d->m_copilot_massage_state;
}

void  SeatControl::copilot_setMassageState(int massageState)
{
    if (d->m_copilot_massage_state == massageState)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_massage_stateKey), QPps::Variant(massageState))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_massage_stateKey);
        return;
    }

    d->m_copilot_massage_state = massageState;
    emit copilot_massageStateChanged(d->m_copilot_massage_state);
}

int SeatControl::copilot_ventilationState() const
{
    return d->m_copilot_ventilation_state;
}

void  SeatControl::copilot_setVentilationState(int ventilationState)
{
    if (d->m_copilot_ventilation_state == ventilationState)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_ventilation_stateKey), QPps::Variant(ventilationState))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_ventilation_stateKey);
        return;
    }

    d->m_copilot_ventilation_state = ventilationState;
    emit copilot_ventilationStateChanged(d->m_copilot_ventilation_state);
}

int SeatControl::main_massageLevel() const
{
    return d->m_main_massage_level;
}

void  SeatControl::main_setMassageLevel(int massagelevel)
{
    if (d->m_main_massage_level == massagelevel)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_massage_levelKey), QPps::Variant(massagelevel))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_massage_levelKey);
        return;
    }

    d->m_main_massage_level = massagelevel;
    emit main_massageLevelChanged(d->m_main_massage_level);
}

int SeatControl::main_ventilationLevel() const
{
    return d->m_main_ventilation_level;
}

void  SeatControl::main_setVentilationLevel(int ventilationlevel)
{
    if (d->m_main_ventilation_level == ventilationlevel)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_ventilation_levelKey), QPps::Variant(ventilationlevel))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_ventilation_levelKey);
        return;
    }

    d->m_main_ventilation_level = ventilationlevel;
    emit main_ventilationLevelChanged(d->m_main_ventilation_level);
}

int SeatControl::copilot_massageLevel() const
{
    return d->m_copilot_massage_level;
}

void  SeatControl::copilot_setMassageLevel(int massagelevel)
{
    if (d->m_copilot_massage_level == massagelevel)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_massage_levelKey), QPps::Variant(massagelevel))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_massage_levelKey);
        return;
    }

    d->m_copilot_massage_level = massagelevel;
    emit copilot_massageLevelChanged(d->m_copilot_massage_level);
}

int SeatControl::copilot_ventilationLevel() const
{
    return d->m_copilot_ventilation_level;
}

void  SeatControl::copilot_setVentilationLevel(int ventilationlevel)
{
    if (d->m_copilot_ventilation_level == ventilationlevel)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_ventilation_levelKey), QPps::Variant(ventilationlevel))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_ventilation_levelKey);
        return;
    }

    d->m_copilot_ventilation_level = ventilationlevel;
    emit copilot_ventilationLevelChanged(d->m_copilot_ventilation_level);
}

QString SeatControl::watchType() const
{
    d->m_watch_type = d->m_ppsObject->attribute(QString::fromLatin1(pps_watch_typeKey)).toString();
    return d->m_watch_type;
}

void SeatControl::setWatchType(QString watchType)
{
    if (d->m_watch_type == watchType)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_watch_typeKey), QPps::Variant(watchType))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_ventilation_levelKey);
        return;
    }

    d->m_watch_type = watchType;
    emit watchTypeChanged(d->m_watch_type);
}

int SeatControl::mainStoreRecallCmd() const
{
    d->m_main_store_recall_position_cmd = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_store_recall_position_cmdKey)).toInt();
    return d->m_main_store_recall_position_cmd;
}

void SeatControl::setMainStoreRecallCmd(int mainStoreRecallCmd)
{
//    if (d->m_main_store_recall_position_cmd == mainStoreRecallCmd)
//        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_store_recall_position_cmdKey), QPps::Variant(mainStoreRecallCmd))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_store_recall_position_cmdKey);
        return;
    }

    d->m_main_store_recall_position_cmd = mainStoreRecallCmd;
    emit mainStoreRecallCmdChanged(d->m_main_store_recall_position_cmd);
}

int SeatControl::copilotStoreRecallCmd() const
{
    d->m_copilot_store_recall_position_cmd = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_store_recall_position_cmdKey)).toInt();
    return d->m_copilot_store_recall_position_cmd;
}

void SeatControl::setCopilotStoreRecallCmd(int copilotStoreRecallCmd)
{
//    if (d->m_copilot_store_recall_position_cmd == copilotStoreRecallCmd)
//        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_store_recall_position_cmdKey), QPps::Variant(copilotStoreRecallCmd))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_store_recall_position_cmdKey);
        return;
    }

    d->m_copilot_store_recall_position_cmd = copilotStoreRecallCmd;
    emit copilotStoreRecallCmdChanged(d->m_copilot_store_recall_position_cmd);
}


int SeatControl::main_slider_movement_status() const
{
    d->m_main_slider_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_slider_movement_statusKey)).toInt();
    return d->m_main_slider_movement_status;
}

void  SeatControl::main_setSliderMovementStatus(int main_slider_movement_status)
{
    if (d->m_main_slider_movement_status == main_slider_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_slider_movement_statusKey), QPps::Variant(main_slider_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_slider_movement_statusKey);
        return;
    }

    d->m_main_slider_movement_status = main_slider_movement_status;
    emit main_slider_movement_statusChanged(d->m_main_slider_movement_status);
}

int SeatControl::main_recliner_movement_status() const
{
    d->m_main_recliner_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_recliner_movement_statusKey)).toInt();
    return d->m_main_recliner_movement_status;
}

void  SeatControl::main_setMainReclinerMovementStatus(int main_recliner_movement_status)
{
    if (d->m_main_recliner_movement_status == main_recliner_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_recliner_movement_statusKey), QPps::Variant(main_recliner_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_recliner_movement_statusKey);
        return;
    }

    d->m_main_recliner_movement_status = main_recliner_movement_status;
    emit main_recliner_movement_statusChanged(d->m_main_recliner_movement_status);
}

int SeatControl::main_tilt_movement_status() const
{
    d->m_main_tilt_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_tilt_movement_statusKey)).toInt();
    return d->m_main_tilt_movement_status;
}

void SeatControl::main_setTiltMovementStatus(int tilt_movement_status)
{
    if (d->m_main_tilt_movement_status == tilt_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_tilt_movement_statusKey), QPps::Variant(tilt_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_tilt_movement_statusKey);
        return;
    }

    d->m_main_tilt_movement_status = tilt_movement_status;
    emit main_tilt_movement_statusChanged(d->m_main_tilt_movement_status);
}

int SeatControl::main_height_movement_status() const
{
    d->m_main_height_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_height_movement_statusKey)).toInt();
    return d->m_main_height_movement_status;
}

void SeatControl::main_setHeightMovementStatus(int main_height_movement_status)
{
    if (d->m_main_height_movement_status == main_height_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_height_movement_statusKey), QPps::Variant(main_height_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_height_movement_statusKey);
        return;
    }

    d->m_main_height_movement_status = main_height_movement_status;
    emit main_height_movement_statusChanged(d->m_main_height_movement_status);
}

int SeatControl::main_lumbar_movement_status() const
{
    d->m_main_lumbar_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_lumbar_movement_statusKey)).toInt();
    return d->m_main_lumbar_movement_status;
}

void SeatControl::main_setLumbarMovementStatus(int main_lumbar_movement_status)
{
    if (d->m_main_lumbar_movement_status == main_lumbar_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_lumbar_movement_statusKey), QPps::Variant(main_lumbar_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_lumbar_movement_statusKey);
        return;
    }

    d->m_main_lumbar_movement_status = main_lumbar_movement_status;
    emit main_lumbar_movement_statusChanged(d->m_main_lumbar_movement_status);
}

int SeatControl::main_slider_position() const
{
    d->m_main_slider_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_slider_positionKey)).toInt();
    return d->m_main_slider_position;
}

void SeatControl::main_setSliderPosition(int main_slider_position)
{
    if (d->m_main_slider_position == main_slider_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_slider_positionKey), QPps::Variant(main_slider_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_slider_positionKey);
        return;
    }

    d->m_main_slider_position = main_slider_position;
    emit main_slider_positionChanged(d->m_main_slider_position);
}

int SeatControl::main_recliner_position() const
{
    d->m_main_recliner_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_recliner_positionKey)).toInt();
    return d->m_main_recliner_position;
}

void SeatControl::main_setReclinerPosition(int main_recliner_position)
{
    if (d->m_main_recliner_position == main_recliner_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_recliner_positionKey), QPps::Variant(main_recliner_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_recliner_positionKey);
        return;
    }

    d->m_main_recliner_position = main_recliner_position;
    emit main_recliner_positionChanged(d->m_main_recliner_position);
}

int SeatControl::main_tilt_position() const
{
    d->m_main_tilt_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_tilt_positionKey)).toInt();
    return d->m_main_tilt_position;
}

void SeatControl::main_setTiltPosition(int main_tilt_position)
{
    if (d->m_main_tilt_position == main_tilt_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_tilt_positionKey), QPps::Variant(main_tilt_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_tilt_positionKey);
        return;
    }

    d->m_main_tilt_position = main_tilt_position;
    emit main_tilt_positionChanged(d->m_main_tilt_position);
}

int SeatControl::main_height_position() const
{
    d->m_main_height_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_height_positionKey)).toInt();
    return d->m_main_height_position;
}

void SeatControl::main_setHeightPosition(int main_height_position)
{
    if (d->m_main_height_position == main_height_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_height_positionKey), QPps::Variant(main_height_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_height_positionKey);
        return;
    }

    d->m_main_height_position = main_height_position;
    emit main_height_positionChanged(d->m_main_height_position);
}

int SeatControl::main_heating_status() const
{
    d->m_main_heatingstatus = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_heatingstatusKey)).toInt();
    return d->m_main_heatingstatus;
}

void SeatControl::main_setHeatingStatus(int main_heating_status)
{
    if (d->m_main_heatingstatus == main_heating_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_heatingstatusKey), QPps::Variant(main_heating_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_heatingstatusKey);
        return;
    }

    d->m_main_heatingstatus = main_heating_status;
    emit main_heating_statusChanged(d->m_main_heatingstatus);
}

int SeatControl::main_ventilation_status() const
{
    d->m_main_ventilation_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_main_ventilation_statusKey)).toInt();
    return d->m_main_ventilation_status;
}

void SeatControl::main_setVentilationStatus(int main_ventilation_status)
{
    if (d->m_main_ventilation_status == main_ventilation_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_main_ventilation_statusKey), QPps::Variant(main_ventilation_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_main_ventilation_statusKey);
        return;
    }

    d->m_main_ventilation_status = main_ventilation_status;
    emit main_ventilation_statusChanged(d->m_main_ventilation_status);
}

int SeatControl::copilot_slider_movement_status() const
{
    d->m_copilot_slider_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_slider_movement_statusKey)).toInt();
    return d->m_copilot_slider_movement_status;
}

void  SeatControl::copilot_setSliderMovementStatus(int copilot_slider_movement_status)
{
    if (d->m_copilot_slider_movement_status == copilot_slider_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_slider_movement_statusKey), QPps::Variant(copilot_slider_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_slider_movement_statusKey);
        return;
    }

    d->m_copilot_slider_movement_status = copilot_slider_movement_status;
    emit copilot_slider_movement_statusChanged(d->m_copilot_slider_movement_status);
}

int SeatControl::copilot_recliner_movement_status() const
{
    d->m_copilot_recliner_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_recliner_movement_statusKey)).toInt();
    return d->m_copilot_recliner_movement_status;
}

void  SeatControl::copilot_setcopilotReclinerMovementStatus(int copilot_recliner_movement_status)
{
    if (d->m_copilot_recliner_movement_status == copilot_recliner_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_recliner_movement_statusKey), QPps::Variant(copilot_recliner_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_recliner_movement_statusKey);
        return;
    }

    d->m_copilot_recliner_movement_status = copilot_recliner_movement_status;
    emit copilot_recliner_movement_statusChanged(d->m_copilot_recliner_movement_status);
}

int SeatControl::copilot_tilt_movement_status() const
{
    d->m_copilot_tilt_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_tilt_movement_statusKey)).toInt();
    return d->m_copilot_tilt_movement_status;
}

void SeatControl::copilot_setTiltMovementStatus(int tilt_movement_status)
{
    if (d->m_copilot_tilt_movement_status == tilt_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_tilt_movement_statusKey), QPps::Variant(tilt_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_tilt_movement_statusKey);
        return;
    }

    d->m_copilot_tilt_movement_status = tilt_movement_status;
    emit copilot_tilt_movement_statusChanged(d->m_copilot_tilt_movement_status);
}

int SeatControl::copilot_height_movement_status() const
{
    d->m_copilot_height_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_height_movement_statusKey)).toInt();
    return d->m_copilot_height_movement_status;
}

void SeatControl::copilot_setHeightMovementStatus(int copilot_height_movement_status)
{
    if (d->m_copilot_height_movement_status == copilot_height_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_height_movement_statusKey), QPps::Variant(copilot_height_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_height_movement_statusKey);
        return;
    }

    d->m_copilot_height_movement_status = copilot_height_movement_status;
    emit copilot_height_movement_statusChanged(d->m_copilot_height_movement_status);
}

int SeatControl::copilot_lumbar_movement_status() const
{
    d->m_copilot_lumbar_movement_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_lumbar_movement_statusKey)).toInt();
    return d->m_copilot_lumbar_movement_status;
}

void SeatControl::copilot_setLumbarMovementStatus(int copilot_lumbar_movement_status)
{
    if (d->m_copilot_lumbar_movement_status == copilot_lumbar_movement_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_lumbar_movement_statusKey), QPps::Variant(copilot_lumbar_movement_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_lumbar_movement_statusKey);
        return;
    }

    d->m_copilot_lumbar_movement_status = copilot_lumbar_movement_status;
    emit copilot_lumbar_movement_statusChanged(d->m_copilot_lumbar_movement_status);
}

int SeatControl::copilot_slider_position() const
{
    d->m_copilot_slider_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_slider_positionKey)).toInt();
    return d->m_copilot_slider_position;
}

void SeatControl::copilot_setSliderPosition(int copilot_slider_position)
{
    if (d->m_copilot_slider_position == copilot_slider_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_slider_positionKey), QPps::Variant(copilot_slider_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_slider_positionKey);
        return;
    }

    d->m_copilot_slider_position = copilot_slider_position;
    emit copilot_slider_positionChanged(d->m_copilot_slider_position);
}

int SeatControl::copilot_recliner_position() const
{
    d->m_copilot_recliner_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_recliner_positionKey)).toInt();
    return d->m_copilot_recliner_position;
}

void SeatControl::copilot_setReclinerPosition(int copilot_recliner_position)
{
    if (d->m_copilot_recliner_position == copilot_recliner_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_recliner_positionKey), QPps::Variant(copilot_recliner_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_recliner_positionKey);
        return;
    }

    d->m_copilot_recliner_position = copilot_recliner_position;
    emit copilot_recliner_positionChanged(d->m_copilot_recliner_position);
}

int SeatControl::copilot_tilt_position() const
{
    d->m_copilot_tilt_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_tilt_positionKey)).toInt();
    return d->m_copilot_tilt_position;
}

void SeatControl::copilot_setTiltPosition(int copilot_tilt_position)
{
    if (d->m_copilot_tilt_position == copilot_tilt_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_tilt_positionKey), QPps::Variant(copilot_tilt_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_tilt_positionKey);
        return;
    }

    d->m_copilot_tilt_position = copilot_tilt_position;
    emit copilot_tilt_positionChanged(d->m_copilot_tilt_position);
}

int SeatControl::copilot_height_position() const
{
    d->m_copilot_height_position = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_height_positionKey)).toInt();
    return d->m_copilot_height_position;
}

void SeatControl::copilot_setHeightPosition(int copilot_height_position)
{
    if (d->m_copilot_height_position == copilot_height_position)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_height_positionKey), QPps::Variant(copilot_height_position))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_height_positionKey);
        return;
    }

    d->m_copilot_height_position = copilot_height_position;
    emit copilot_height_positionChanged(d->m_copilot_height_position);
}

int SeatControl::copilot_heating_status() const
{
    d->m_copilot_heatingstatus = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_heatingstatusKey)).toInt();
    return d->m_copilot_heatingstatus;
}

void SeatControl::copilot_setHeatingStatus(int copilot_heating_status)
{
    if (d->m_copilot_heatingstatus == copilot_heating_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_heatingstatusKey), QPps::Variant(copilot_heating_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_heatingstatusKey);
        return;
    }

    d->m_copilot_heatingstatus = copilot_heating_status;
    emit copilot_heating_statusChanged(d->m_copilot_heatingstatus);
}

int SeatControl::copilot_ventilation_status() const
{
    d->m_copilot_ventilation_status = d->m_ppsObject->attribute(QString::fromLatin1(pps_copilot_ventilation_statusKey)).toInt();
    return d->m_copilot_ventilation_status;
}

void SeatControl::copilot_setVentilationStatus(int copilot_ventilation_status)
{
    if (d->m_copilot_ventilation_status == copilot_ventilation_status)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_copilot_ventilation_statusKey), QPps::Variant(copilot_ventilation_status))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_copilot_ventilation_statusKey);
        return;
    }

    d->m_copilot_ventilation_status = copilot_ventilation_status;
    emit copilot_ventilation_statusChanged(d->m_main_ventilation_status);
}



}
