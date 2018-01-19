#ifndef QTQNXCAR2_CAMERACONTROL_P_H
#define QTQNXCAR2_CAMERACONTROL_P_H

#include "CameraControl.h"

#include <qpps/object.h>

namespace QnxCar {

class CameraControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(CameraControl *qq);

    CameraControl *q;

    QPps::Object *m_ppsObject;

    QString m_capture;
    QString m_direct;
    QString m_left_port;
    QString m_right_port;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
