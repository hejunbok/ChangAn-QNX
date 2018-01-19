#ifndef QTQNXCAR2_PHONE_P_H
#define QTQNXCAR2_PHONE_P_H

#include "Phone.h"

#include <qpps/object.h>

#include <QTimer>
#include <QElapsedTimer>

namespace QnxCar {

class Phone::Private : public QObject
{
    Q_OBJECT

public:
    Private(Phone *qq);
    Phone *q;

    QString currentCallId(const QString &param) const;

    QPps::Object *m_ppsControlObject;
    QPps::Object *m_ppsStatusObject;

    Phone::State m_state;
    QString m_currentCallId;
    QString m_lastNumber;

    QElapsedTimer m_callDuration;
    QTimer m_callDurationTimer;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
