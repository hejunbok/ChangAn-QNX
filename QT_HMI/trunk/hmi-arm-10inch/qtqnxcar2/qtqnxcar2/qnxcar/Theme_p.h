#ifndef QTQNXCAR2_THEME_P_H
#define QTQNXCAR2_THEME_P_H

#include "Theme.h"

#include <qpps/object.h>

namespace QnxCar {

class Theme::Private : public QObject
{
    Q_OBJECT

public:
    Private(Theme *qq);

    Theme *q;

    QPps::Object *m_ppsObject;

    QString m_theme;
    Mode m_mode;
    QString m_assetsDirectory;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
