#include "MusicControl.h"
#include "MusicControl_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_musicControlCmd = "operation";
static const char* pps_multimediaControlCmd = "multimedia";

QString MUSIC_CONTROL[5] = {"none", "prev", "next", "play", "pause"};
QString MULTIMEDIA_CONTROL[4] = {"none","radio", "music", "video"};

MusicControl::Private::Private(MusicControl *qq)
    : QObject(qq)
    , q(qq)
    , m_musicControlCmd(0)
    , m_multimedia("none")
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/music_control"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_musicControlCmd = q->getIndexByMusicControl(m_ppsObject->attribute(QString::fromLatin1(pps_musicControlCmd)).toString());
        m_multimedia = m_ppsObject->attribute(QString::fromLatin1(pps_multimediaControlCmd)).toString();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void MusicControl::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_musicControlCmd)) {
        if(attribute.toString() == "none"){
            if(m_musicControlCmd != 0){
                m_musicControlCmd = 0;
                emit q->musicControlCmdChanged(m_musicControlCmd);
            }
        }
        else if(attribute.toString() == "prev"){
            if(m_musicControlCmd != 1){
                m_musicControlCmd = 1;
                emit q->musicControlCmdChanged(m_musicControlCmd);
            }
        }
        else if(attribute.toString() == "next"){
            if(m_musicControlCmd != 2){
                m_musicControlCmd = 2;
                emit q->musicControlCmdChanged(m_musicControlCmd);
            }
        }
        else if(attribute.toString() == "play"){
            if(m_musicControlCmd != 3){
                m_musicControlCmd = 3;
                emit q->musicControlCmdChanged(m_musicControlCmd);
            }
        }
        else if(attribute.toString() == "pause"){
            if(m_musicControlCmd != 4){
                m_musicControlCmd = 4;
                emit q->musicControlCmdChanged(m_musicControlCmd);
            }
        }
    }
    else if (name == QString::fromLatin1(pps_multimediaControlCmd)) {
        if (m_multimedia != attribute.toString()) {
            m_multimedia = attribute.toString();
            emit q->multimediaControlCmdChanged(m_multimedia);
        }
    }
}


MusicControl::MusicControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int MusicControl::getIndexByMusicControl(QString controlcmd)
{
    int sizeOfArray = sizeof(MUSIC_CONTROL)/sizeof(MUSIC_CONTROL[0]);

    for(int i = 0; i < sizeOfArray; i++) {
        if(MUSIC_CONTROL[i] == controlcmd)
            return i;
    }
    // in case nothing found
    return -1;
}

int MusicControl::musicControlCmd() const
{
    return d->m_musicControlCmd;
}

void MusicControl::setMusicControlCmd(int musicControlCmd)
{
    if (d->m_musicControlCmd == musicControlCmd)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_musicControlCmd), QPps::Variant(MUSIC_CONTROL[musicControlCmd]))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_musicControlCmd);
        return;
    }

    d->m_musicControlCmd = musicControlCmd;
    emit musicControlCmdChanged(d->m_musicControlCmd);
}

QString MusicControl::multimediaControlCmd() const
{
    return d->m_multimedia;
}

void MusicControl::setMultimediaControlCmd(QString multimediaControlCmd)
{
    if (d->m_multimedia == multimediaControlCmd)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_multimediaControlCmd), QPps::Variant(multimediaControlCmd))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_multimediaControlCmd);
        return;
    }

    d->m_multimedia = multimediaControlCmd;
    emit multimediaControlCmdChanged(d->m_multimedia);
}

}
