#include "KeyboardServer.h"
#include "KeyboardServer_p.h"

#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>

#include <qpps/changeset.h>

namespace QnxCar {

KeyboardServer::Private::Private(KeyboardServer *qq)
    : QObject(qq)
    , q(qq)
    , capsLock(false)
    , height(0)
    , visible(false)
    , showPopup(false)
{
    ppsControl = new QPps::Object(QStringLiteral("/pps/system/keyboard/control"),
                                  QPps::Object::PublishAndSubscribeMode, true, this);
    ppsStatus = new QPps::Object(QStringLiteral("/pps/system/keyboard/status"),
                                 QPps::Object::PublishMode, true, this);

    if (ppsStatus->isValid()) {
        writeStatusVisible();
        writeStatusSize();
        writeStatusShowPopup();
        writeStatusLayout();
        writeStatusAvailableLayouts();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS status object:" << ppsStatus->errorString();
    }
    if (ppsControl->isValid()) {
        connect(ppsControl, SIGNAL(attributeChanged(QString,QPps::Variant)), this, SLOT(controlAttributeChanged(QString,QPps::Variant)));
        ppsControl->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS control object:" << ppsControl->errorString();
    }
}

void KeyboardServer::Private::writeStatusVisible()
{
    if (!ppsStatus->setAttribute(QStringLiteral("visible"), QPps::Variant(visible)))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();
}

void KeyboardServer::Private::writeStatusSize()
{
    if (!ppsStatus->setAttribute(QStringLiteral("size"), QPps::Variant(height)))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();
}

void KeyboardServer::Private::writeStatusShowPopup()
{
    if (!ppsStatus->setAttribute(QStringLiteral("showPopup"), QPps::Variant(showPopup)))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();
}

void KeyboardServer::Private::writeStatusLayout()
{
    if (!ppsStatus->setAttribute(QStringLiteral("layout"), QPps::Variant(layout)))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();
}

void KeyboardServer::Private::writeStatusAvailableLayouts()
{
    if (!ppsStatus->setAttribute(QStringLiteral("availableLayouts"), QPps::Variant(QJsonDocument::fromVariant(availableLayouts))))
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << ppsStatus->errorString();
}

void KeyboardServer::Private::controlAttributeChanged(const QString &name, const QPps::Variant &value)
{
    if (name == QStringLiteral("msg")) {
        const QString msg = value.toString();

        if (msg == QStringLiteral("show")) {
            emit q->show();
            visible = true;
            QPps::Changeset changes;
            changes.assignments.insert(QStringLiteral("visible"), QPps::Variant(true));
            changes.assignments.insert(QStringLiteral("size"), QPps::Variant(height));
            ppsStatus->setAttributes(changes);
            //TODO send reply
        } else if (msg == QStringLiteral("hide")) {
            emit q->hide();
            visible = false;
            QPps::Changeset changes;
            changes.assignments.insert(QStringLiteral("visible"), QPps::Variant(false));
            changes.assignments.insert(QStringLiteral("size"), QPps::Variant(0));
            ppsStatus->setAttributes(changes);
            //TODO send reply
        } else {
            //TODO send error reply unknown msg
        }
    }
}

KeyboardServer::KeyboardServer(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

bool KeyboardServer::capsLock() const
{
    return d->capsLock;
}

void KeyboardServer::setCapsLock(bool capsLock)
{
    if (d->capsLock == capsLock)
        return;
    d->capsLock = capsLock;
    emit capsLockChanged(capsLock);
}

void KeyboardServer::setHeight(int height)
{
    if (d->height == height)
        return;
    d->height = height;
    if (d->visible)
        d->writeStatusSize();
}

int KeyboardServer::height() const
{
    return d->height;
}

void KeyboardServer::setShowPopup(bool showPopup)
{
    if (showPopup == d->showPopup)
        return;
    d->showPopup = showPopup;
    d->writeStatusShowPopup();
}

bool KeyboardServer::showPopup() const
{
    return d->showPopup;
}

QString KeyboardServer::layout() const
{
    return d->layout;
}

void KeyboardServer::setLayout(const QString &layout)
{
    if (layout == d->layout)
        return;
    d->layout = layout;
    d->writeStatusLayout();
}

QStringList KeyboardServer::availableLayouts() const
{
    return d->availableLayouts;
}

void KeyboardServer::setAvailableLayouts(const QStringList &availableLayouts)
{
    if (availableLayouts == d->availableLayouts)
        return;
    d->availableLayouts = availableLayouts;
    d->writeStatusAvailableLayouts();
}

void KeyboardServer::sendKeyCode(int keyCode)
{
    QJsonObject dat;
    dat.insert(QStringLiteral("keysym"), keyCode);

    QPps::Changeset changes;
    changes.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("key")));
    changes.assignments.insert(QStringLiteral("dat"), QPps::Variant(QJsonDocument(dat)));
    if (!d->ppsControl->setAttributes(changes)) {
        qWarning() << Q_FUNC_INFO << "Could not write to pps" << d->ppsControl->errorString();
    }
}

}
