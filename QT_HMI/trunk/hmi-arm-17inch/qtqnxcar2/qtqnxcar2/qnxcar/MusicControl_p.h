#ifndef QTQNXCAR2_MUSIC_CONTROL_P_H
#define QTQNXCAR2_MUSIC_CONTROL_P_H

#include "MusicControl.h"

#include <qpps/object.h>

namespace QnxCar {

class MusicControl::Private : public QObject
{
    Q_OBJECT

public:
    Private(MusicControl *qq);

    MusicControl *q;

    QPps::Object *m_ppsObject;

    int m_musicControlCmd;
    QString m_multimedia;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
