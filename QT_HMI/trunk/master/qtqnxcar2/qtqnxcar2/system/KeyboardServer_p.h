#ifndef QTQNXCAR2_KEYBOARDSERVER_P_H
#define QTQNXCAR2_KEYBOARDSERVER_P_H

#include "KeyboardServer.h"

#include <qpps/object.h>

#include <QStringList>

namespace QnxCar {

class KeyboardServer::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(KeyboardServer *qq);

    KeyboardServer * const q;
    QPps::Object *ppsControl;
    QPps::Object *ppsStatus;
    bool capsLock;
    int height;
    bool visible;
    bool showPopup;
    QString layout;
    QStringList availableLayouts;

    void writeStatusShowPopup();
    void writeStatusLayout();
    void writeStatusAvailableLayouts();
    void writeStatusVisible();
    void writeStatusSize();

private Q_SLOTS:
    void controlAttributeChanged(const QString &name, const QPps::Variant &variant);
};

}

#endif
