#ifndef QTQNXCAR2_THEMECONTROL_P_H
#define QTQNXCAR2_THEMECONTROL_P_H

#include "ThemeControl.h"

#include <qpps/object.h>

namespace QnxCar {

class ThemeControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(ThemeControl *qq);

    ThemeControl *q;

    QPps::Object *m_ppsObject;

    QString m_theme;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
