#ifndef QTQNXCAR2_SETTINGS_P_H
#define QTQNXCAR2_SETTINGS_P_H

#include "Settings.h"

#include <qpps/object.h>

namespace QnxCar {

class Settings::Private : public QObject
{
    Q_OBJECT

public:
    Private(Settings *qq);

    Settings *q;

    QPps::Object *m_ppsObject;

    Profile m_appSectionProfile;
    Profile m_carControlProfile;
    Profile m_communicationProfile;
    Profile m_mediaPlayerProfile;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
