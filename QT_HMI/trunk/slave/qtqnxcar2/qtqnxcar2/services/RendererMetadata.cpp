#include "RendererMetadata_p.h"

#include <QDebug>

#include <qpps/changeset.h>

using namespace QnxCar;

static const QString md_title_bitrate = QStringLiteral("md_title_bitrate");
static const QString md_title_duration = QStringLiteral("md_title_duration");
static const QString md_title_mediatype = QStringLiteral("md_title_mediatype");
static const QString md_title_samplerate = QStringLiteral("md_title_samplerate");
static const QString md_title_seekable = QStringLiteral("md_title_seekable");
static const QString md_video_width = QStringLiteral("md_video_width");
static const QString md_video_height = QStringLiteral("md_video_height");
static const QString md_video_pixel_width = QStringLiteral("md_video_pixel_width");
static const QString md_video_pixel_height = QStringLiteral("md_video_pixel_height");
static const QString key_url = QStringLiteral("url");

RendererMetadata::RendererMetadata(const QString &context, QObject *parent)
    : QObject(parent)
    , m_ppsMetadata(QStringLiteral("/pps/services/multimedia/renderer/context/%1/metadata").arg(context), QPps::Object::SubscribeMode)
{
    if (m_ppsMetadata.isValid()) {
        m_ppsMetadata.setAttributeCacheEnabled(true);
        connect(&m_ppsMetadata, &QPps::Object::attributesChanged, this, &RendererMetadata::attributesChanged);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsMetadata.errorString();
    }
}

RendererMetadata::~RendererMetadata()
{
}

// we do toString().toInt() etc. as these attributes don't have encoding 'n'

int RendererMetadata::bitrate() const
{
    return m_ppsMetadata.attribute(md_title_bitrate).toString().toInt();
}

int RendererMetadata::duration() const
{
    return m_ppsMetadata.attribute(md_title_duration).toString().toInt();
}

int RendererMetadata::mediaType() const
{
    return m_ppsMetadata.attribute(md_title_mediatype).toString().toInt();
}

int RendererMetadata::sampleRate() const
{
    return m_ppsMetadata.attribute(md_title_samplerate).toString().toInt();
}

bool RendererMetadata::seekable() const
{
    return m_ppsMetadata.attribute(md_title_seekable).toString() == QStringLiteral("1");
}

int RendererMetadata::videoWidth() const
{
    return m_ppsMetadata.attribute(md_video_width).toString().toInt();
}

int RendererMetadata::videoHeight() const
{
    return m_ppsMetadata.attribute(md_video_height).toString().toInt();
}

int RendererMetadata::videoPixelWidth() const
{
    return m_ppsMetadata.attribute(md_video_pixel_width).toString().toInt();
}

int RendererMetadata::videoPixelHeight() const
{
    return m_ppsMetadata.attribute(md_video_pixel_height).toString().toInt();
}

QString RendererMetadata::url() const
{
    return m_ppsMetadata.attribute(key_url).toString();
}

void RendererMetadata::attributesChanged(const QPps::Changeset &changes)
{
    QMap<QString,QPps::Variant>::ConstIterator it = changes.assignments.constBegin();
    for ( ; it != changes.assignments.constEnd(); ++it)
    {
        if (it.key() == md_title_bitrate)
            emit bitrateChanged(bitrate());
        else if (it.key() == md_title_duration)
            emit durationChanged(duration());
        else if (it.key() == md_title_mediatype)
            emit mediaTypeChanged(mediaType());
        else if (it.key() == md_title_samplerate)
            emit sampleRateChanged(sampleRate());
        else if (it.key() == md_title_seekable)
            emit seekableChanged(seekable());
        else if (it.key() == md_video_width)
            emit videoWidthChanged(videoWidth());
        else if (it.key() == md_video_height)
            emit videoHeightChanged(videoHeight());
        else if (it.key() == md_video_pixel_width)
            emit videoPixelWidthChanged(videoPixelWidth());
        else if (it.key() == md_video_pixel_height)
            emit videoPixelHeightChanged(videoPixelHeight());
        else if (it.key() == key_url)
            emit urlChanged(url());
    }

    emit changed();
}
