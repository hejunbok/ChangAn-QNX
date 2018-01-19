#include "Theme.h"
#include "Theme_p.h"

#include <QDebug>
#include <QFile>

namespace QnxCar {

static const char* pps_nightModeKey = "nightMode";
static const char* pps_themeKey = "theme";

Theme::Private::Private(Theme *qq)
    : QObject(qq)
    , q(qq)
    , m_theme(Theme::defaultTheme())
    , m_mode(Day)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/profile/theme"),
                                   QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_mode = (m_ppsObject->attribute(QString::fromLatin1(pps_nightModeKey)).toBool() ? Night : Day);
        m_theme = m_ppsObject->attribute(QString::fromLatin1(pps_themeKey)).toString();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

QString Theme::defaultTheme()
{
    return QStringLiteral("technology");
//    return QStringLiteral("default");
}

void Theme::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_nightModeKey)) {
        const Mode mode = (attribute.toBool() ? Night : Day);
        if (m_mode == mode)
            return;

        m_mode = mode;
        emit q->modeChanged(m_mode);

    } else if (name == QString::fromLatin1(pps_themeKey)) {
        const QString theme = attribute.toString();
        if (m_theme == theme)
            return;

        m_theme = theme;
        emit q->themeChanged(m_theme);
    }
}


Theme::Theme(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

QString Theme::current() const
{
    return d->m_theme;
    //return "default";
}

void Theme::setCurrent(const QString& theme)
{
    if (d->m_theme == theme)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_themeKey), QPps::Variant(theme))) {
        qWarning() << "Theme: unable to write back 'theme'";
        return;
    }

    d->m_theme = theme;
    emit themeChanged(d->m_theme);
}

Theme::Mode Theme::mode() const
{
    return d->m_mode;
}

void Theme::setMode(Mode mode)
{
    if (d->m_mode == mode)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_nightModeKey), QPps::Variant(mode == Night))) {
        qWarning() << "Theme: unable to write back 'nightMode'";
        return;
    }

    d->m_mode = mode;
    emit modeChanged(d->m_mode);
}

}
