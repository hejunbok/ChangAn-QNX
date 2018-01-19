#ifndef QTQNXCAR2_APPLAUNCHER_P_H
#define QTQNXCAR2_APPLAUNCHER_P_H

#include "AppLauncher.h"

#include <qpps/object.h>

class QnxCar::AppLauncher::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(AppLauncher *qq);

    AppLauncher *q;
    QPps::Object *m_ppsObject;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

#endif
