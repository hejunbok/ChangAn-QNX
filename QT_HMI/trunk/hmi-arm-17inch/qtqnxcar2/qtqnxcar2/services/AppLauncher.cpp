#include "AppLauncher.h"
#include "AppLauncher_p.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace QnxCar;

AppLauncher::Private::Private(AppLauncher *qq)
    : QObject(qq)
    , q(qq)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/services/app-launcher"),
                                   QPps::Object::SubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        //workaround: app-launcher persists the last launch among board restarts,
        //so avoid emitting the initial content by enabling the cache before connect
        m_ppsObject->setAttributeCacheEnabled(true);
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void AppLauncher::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QStringLiteral("req")) {
        const QJsonObject req = attribute.toJson().object();
        if (req.value(QStringLiteral("cmd")).toString() == QStringLiteral("launch app")) {
            const QString app = req.value(QStringLiteral("app")).toString();
            const QJsonValue dat = req.value(QStringLiteral("dat"));
            emit q->launchApp(app, dat);
            return;
        } else if (req.value(QStringLiteral("cmd")).toString() == QStringLiteral("close app")) {
            const QString app = req.value(QStringLiteral("app")).toString();
            emit q->closeApp(app);
            return;
        }
    }

    qWarning("%s: Not handled: %s:%s:%s", Q_FUNC_INFO, qPrintable(name), attribute.encoding().constData(), attribute.value().constData());
}

AppLauncher::AppLauncher(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

