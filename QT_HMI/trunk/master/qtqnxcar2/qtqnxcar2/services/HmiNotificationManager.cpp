#include "HmiNotificationManager.h"
#include "HmiNotificationManager_p.h"

#include <qpps/changeset.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

#include "qpps/dirwatcher.h"

namespace QnxCar {
/**
 * Static function to convert incoming state values from PPS
 * to the proper state ennumerations supported by the HmiNotificationManager service
 */
static HmiNotificationManager::State stringToState(const QString &stringData){
    static QHash<QString, HmiNotificationManager::State> hash;
    if(hash.isEmpty()){
        hash.insert("HNM_ERROR", HmiNotificationManager::ErrorState);
        hash.insert("Home", HmiNotificationManager::Home);
        hash.insert("Navigation", HmiNotificationManager::Navigation);
        hash.insert("MediaPlayer", HmiNotificationManager::MediaPlayer);
        hash.insert("CarControl", HmiNotificationManager::CarControl);
        hash.insert("Communication", HmiNotificationManager::Communication);
        hash.insert("AppSection", HmiNotificationManager::AppSection);
    }
    const QHash<QString, HmiNotificationManager::State>::const_iterator it = hash.constFind(stringData);
    if (it == hash.constEnd()) {
        qWarning() << Q_FUNC_INFO <<": unknown state:" << stringData;
        return HmiNotificationManager::ErrorState;
    }

    return *it;

}
static HmiNotificationManager::MessageType stringToMessageType(const QString &stringData){
    static QHash<QString, HmiNotificationManager::MessageType> hash;

    if(hash.isEmpty()){
        hash.insert("HNM_UNSUPPORTED _MESSAGE_TYPE", HmiNotificationManager::UnsupportedType);
        hash.insert("Growl", HmiNotificationManager::Growl);
    }
    const QHash<QString, HmiNotificationManager::MessageType>::const_iterator it = hash.constFind(stringData);
    if (it == hash.constEnd()) {
        qWarning() << Q_FUNC_INFO<< "unknown message type:" <<stringData;
        return HmiNotificationManager::UnsupportedType;
    }
    return *it;
}

HmiNotificationManager::Private::Private(HmiNotificationManager *qq)
    : QObject(qq)
    , q(qq)
{
    m_ppsObjectDirWatcher = new QPps::DirWatcher(QStringLiteral("/pps/services/hmi-notification"), this);

    connect(m_ppsObjectDirWatcher, &QPps::DirWatcher::objectAdded, this, &Private::connectPPSObjects);
}
void HmiNotificationManager::Private::connectPPSObjects(const QString &object){

    if(object == QStringLiteral("Messaging")){
        m_ppsMessageObject = new QPps::Object(QStringLiteral("/pps/services/hmi-notification/Messaging"),
                                                QPps::Object::PublishAndSubscribeMode, false, this);
        if (m_ppsMessageObject->isValid()) {
            connect(m_ppsMessageObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsMessageObjectAttributeChanged(QString,QPps::Variant)));
            m_ppsMessageObject->setAttributeCacheEnabled(true);
        }else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsMessageObject->errorString();
        }

    }else if(object == QStringLiteral("Status")){
        m_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/services/hmi-notification/Status"),
                                              QPps::Object::SubscribeMode, true, this);
        if (m_ppsStatusObject->isValid()) {
            connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
            m_ppsStatusObject->setAttributeCacheEnabled(true);

        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
        }
    }
}

void HmiNotificationManager::Private::ppsAttributeChanged(const QString &attributeName, const QPps::Variant &attribute)
{
    QJsonArray  array = attribute.toJson().array();
    QString view, name, type;
    if(array.size() > 0){
        QJsonObject obj = array[0].toObject();
        view = obj["view"].toString();
        name = obj["name"].toString();
        type = obj["type"].toString();

        // use the name for now, this is subject to change
        // since it was originally intended that the view
        // holds the associated value
        const State state = stringToState(name);

        switch (state) {
            case Home:
            case Navigation:
            case MediaPlayer:
            case CarControl:
            case Communication:
            case AppSection:
            {
                emit q->stateChanged(state, view, type);
            }
            break;
            default:
            {
                qWarning()<< Q_FUNC_INFO << " HNM Status Unrecognized State: " << view << ": " << name <<": "<< type;
            }
            break;
        }
    }else {
        qWarning() << Q_FUNC_INFO << " No data or error in the JSON attribute" << attribute;
    }

}

void HmiNotificationManager::Private::ppsMessageObjectAttributeChanged (const QString &attributeName, const QPps::Variant &attribute){
    QJsonArray array = attribute.toJson().array();
    QString name, type;
    if(array.size() > 0) {
        QJsonObject obj = array[0].toObject();
        name = obj["name"].toString();
        type = obj["type"].toString();

        const MessageType messageType= stringToMessageType(type);

        switch (messageType) {
            case Growl:
            {
                emit q->messageReceived(HmiNotificationManager::Growl, name);
                break;
            }
            default:
            {
                qWarning()<< Q_FUNC_INFO << "HNM Message Unrecognized Message Type:"<<type<<"Name:"<<name;
            }
        }
    }else {
        qWarning() << Q_FUNC_INFO << "No data or error in the JSON atttribute" << attribute;
    }
}

HmiNotificationManager::HmiNotificationManager(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}


}
