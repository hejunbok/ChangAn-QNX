#include "Keyboard.h"
#include "Keyboard_p.h"

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <qpps/changeset.h>

using namespace QPps;

namespace QnxCar {

Keyboard::Private::Private(Keyboard *qq)
    : QObject(qq)
    , q(qq)
{
    ppsControl = new Object(QStringLiteral("/pps/system/keyboard/control"),
                            Object::PublishAndSubscribeMode, this);
    ppsStatus = new Object(QStringLiteral("/pps/system/keyboard/status"),
                            Object::SubscribeMode, this);

    if (ppsStatus->isValid()) {
        ppsStatus->setAttributeCacheEnabled(true);
        connect(ppsStatus, SIGNAL(attributeChanged(QString,QPps::Variant)), this, SLOT(statusAttributeChanged(QString,QPps::Variant)));
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS status object:" << ppsStatus->errorString();
    }
    if (ppsControl->isValid()) {
        ppsControl->setAttributeCacheEnabled(true);
        connect(ppsControl, SIGNAL(attributesChanged(QPps::Changeset)), this, SLOT(controlObjectChanged()));
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS control object:" << ppsControl->errorString();
    }
}

static QChar charFromKeysym(int keysym)
{
    return QChar(keysym);
}

void Keyboard::Private::controlObjectChanged()
{
    if (ppsControl->attribute(QStringLiteral("msg")).toString() == QStringLiteral("key")) {
        const Variant dat = ppsControl->attribute(QStringLiteral("dat"));
        const QJsonObject hash = dat.toJson().object();
        const int keysym = hash.value(QStringLiteral("keysym")).toDouble();

        switch (keysym) {
        case Key_Delete:
        case Key_Return:
            q->specialKeyPressed(static_cast<SpecialKey>(keysym));
            break;
        default:
            q->keyPressed(charFromKeysym(keysym));
            break;
        }
    }
}

static QStringList parseLayoutsAvailable(const QJsonDocument &doc)
{
    const QJsonArray array = doc.array();
    QStringList result;
    result.reserve(array.size());
    Q_FOREACH (const QJsonValue& i, array)
        result << i.toString();
    return result;
}

void Keyboard::Private::statusAttributeChanged(const QString &name, const QPps::Variant &value)
{
    if (name == QStringLiteral("size")) {
        emit q->sizeChanged(value.toInt());
    } else if (name == QStringLiteral("visible")) {
        emit q->visibleChanged(value.toBool());
    } else if (name == QStringLiteral("showPopup")) {
        emit q->showPopupChanged(value.toBool());
    } else if (name == QStringLiteral("layout")) {
        emit q->layoutChanged(value.toString());
    } else if (name == QStringLiteral("layoutsAvailable")) {
        layoutsAvailable = parseLayoutsAvailable(value.toJson());
        emit q->layoutsAvailableChanged(layoutsAvailable);
    } else {

    }
}

Keyboard::Keyboard(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

QString Keyboard::layout() const
{
    return d->ppsStatus->attribute(QStringLiteral("layout")).toString();
}

QStringList Keyboard::layoutsAvailable() const
{
    return d->layoutsAvailable;
}

bool Keyboard::visible() const
{
    return d->ppsStatus->attribute(QStringLiteral("visible"), Variant(false)).toBool();
}

bool Keyboard::showPopup() const
{
    return d->ppsStatus->attribute(QStringLiteral("showPopup"), Variant(false)).toBool();
}

int Keyboard::size() const
{
    return d->ppsStatus->attribute(QStringLiteral("size"), Variant(0)).toInt();
}

void Keyboard::show()
{
    QPps::Changeset changes;
    changes.assignments.insert(QStringLiteral("msg"), Variant(QStringLiteral("show")));
    changes.assignments.insert(QStringLiteral("dat"), Variant(QByteArrayLiteral("{}"), "json"));
    if (!d->ppsControl->setAttributes(changes)) {
        qWarning() << Q_FUNC_INFO << "Could not write to pps:" << d->ppsControl->errorString();
    }
}

void Keyboard::hide()
{
    QPps::Changeset changes;
    changes.assignments.insert(QStringLiteral("msg"), Variant(QStringLiteral("show")));
    changes.assignments.insert(QStringLiteral("dat"), Variant(QByteArrayLiteral("{}"), "json"));
    if (!d->ppsControl->setAttributes(changes)) {
        qWarning() << Q_FUNC_INFO << "Could not write to pps:" << d->ppsControl->errorString();
    }
}

}
