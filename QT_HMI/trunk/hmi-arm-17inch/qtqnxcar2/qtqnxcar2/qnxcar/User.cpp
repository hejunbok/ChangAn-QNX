#include "User.h"
#include "User_p.h"

#include <QDebug>

namespace QnxCar {

static const char* pps_idKey = "id";
static const char* pps_fullNameKey = "fullName";
static const char* pps_avatarKey = "avatar";

User::Private::Private(User *qq)
    : QObject(qq)
    , q(qq)
    , m_id(1)
    , m_fullName(QStringLiteral("Default"))
    , m_avatar(QStringLiteral("male1"))
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/profile/user"),
                                   QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_id = m_ppsObject->attribute(QString::fromLatin1(pps_idKey)).toInt();
        m_fullName = m_ppsObject->attribute(QString::fromLatin1(pps_fullNameKey)).toString();
        m_avatar = m_ppsObject->attribute(QString::fromLatin1(pps_avatarKey)).toString();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void User::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_idKey)) {
        if (m_id != attribute.toInt()) {
            m_id = attribute.toInt();
            emit q->idChanged(m_id);
        }
    } else if (name == QString::fromLatin1(pps_fullNameKey)) {
        if (m_fullName != attribute.toString()) {
            m_fullName = attribute.toString();
            emit q->fullNameChanged(m_fullName);
        }
    } else if (name == QString::fromLatin1(pps_avatarKey)) {
        if (m_avatar != attribute.toString()) {
            m_avatar = attribute.toString();
            emit q->avatarChanged(m_avatar);
        }
    }
}


User::User(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int User::id() const
{
    return d->m_id;
}

void User::setId(int rawId)
{
    const int id = qMax(0, rawId);

    if (d->m_id == id)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_idKey), QPps::Variant(id))) {
        qWarning() << "User: unable to write back 'id'";
        return;
    }

    d->m_id = id;
    emit idChanged(d->m_id);
}

QString User::fullName() const
{
    return d->m_fullName;
}

void User::setFullName(const QString &fullName)
{
    if (d->m_fullName == fullName)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fullNameKey), QPps::Variant(fullName))) {
        qWarning() << "User: unable to write back 'fullName'";
        return;
    }

    d->m_fullName = fullName;
    emit fullNameChanged(d->m_fullName);
}

QString User::avatar() const
{
    return d->m_avatar;
}

void User::setAvatar(const QString &rawAvatar)
{
    const QString avatar = (rawAvatar != QStringLiteral("male1") &&
                            rawAvatar != QStringLiteral("male2") &&
                            rawAvatar != QStringLiteral("female1") &&
                            rawAvatar != QStringLiteral("female2") ? QStringLiteral("male1")
                                                                   : rawAvatar);

    if (d->m_avatar == avatar)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_avatarKey), QPps::Variant(avatar))) {
        qWarning() << "User: unable to write back 'avatar'";
        return;
    }

    d->m_avatar = avatar;
    emit avatarChanged(d->m_avatar);
}

}
