#include "ThemeControl.h"
#include "ThemeControl_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_currentThemeKey = "theme";

ThemeControl::Private::Private(ThemeControl *qq)
    : QObject(qq)
    , q(qq)
    , m_theme("technology")
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/theme_control"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_theme = m_ppsObject->attribute(QString::fromLatin1(pps_currentThemeKey)).toString();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void ThemeControl::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_currentThemeKey)) {
        if (m_theme != attribute.toString()) {
            m_theme = attribute.toString();
            emit q->currentThemeChanged(m_theme);
        }
    }
}

ThemeControl::ThemeControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

QString ThemeControl::getCurrentTheme() const
{
    d->m_theme = d->m_ppsObject->attribute(QString::fromLatin1(pps_currentThemeKey)).toString();

    return d->m_theme;
}

void ThemeControl::setCurrentTheme(QString currentTheme)
{
    if (d->m_theme == currentTheme){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_currentThemeKey), QPps::Variant(currentTheme))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_currentThemeKey);
        return;
    }

    d->m_theme = currentTheme;
    emit currentThemeChanged(d->m_theme);
}


}
