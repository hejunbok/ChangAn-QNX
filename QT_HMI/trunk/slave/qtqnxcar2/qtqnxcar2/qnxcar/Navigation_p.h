#ifndef QTQNXCAR2_NAVIGATION_P_H
#define QTQNXCAR2_NAVIGATION_P_H

#include "Navigation.h"
#include "Location.h"
#include "ManeuversModel.h"
#include "pps/ControlCommand.h"

#include <qpps/object.h>

namespace QnxCar {

class Navigation::Private : public QObject
{
    Q_OBJECT

    Navigation *const q;
public:
    explicit Private(Navigation *qq);

    QPps::Object ppsControl;
    QPps::Object ppsStatus;
    ManeuversModel maneuversModel;
    Location destination;

public Q_SLOTS:
    void statusChanged(const QString &name, const QPps::Variant &value);
    void searchFinished(QnxCar::ControlCommand *command);
    void getPointsOfInterestFinished(QnxCar::ControlCommand *command);
};

}

#endif
