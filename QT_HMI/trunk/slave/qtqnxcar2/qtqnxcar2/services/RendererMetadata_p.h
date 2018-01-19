#ifndef QTQNXCAR2_RENDERERMETADATA_P_H
#define QTQNXCAR2_RENDERERMETADATA_P_H

#include <QObject>

#include <qpps/object.h>

namespace QnxCar {

class RendererMetadata : public QObject {
    Q_OBJECT
    Q_PROPERTY(int bitrate READ bitrate NOTIFY bitrateChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int mediaType READ mediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(int sampleRate READ sampleRate NOTIFY sampleRateChanged)
    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)
    Q_PROPERTY(int videoWidth READ videoWidth NOTIFY videoWidthChanged)
    Q_PROPERTY(int videoHeight READ videoHeight NOTIFY videoHeightChanged)
    Q_PROPERTY(int videoPixelWidth READ videoPixelWidth NOTIFY videoPixelWidthChanged)
    Q_PROPERTY(int videoPixelHeight READ videoPixelHeight NOTIFY videoPixelHeightChanged)
    Q_PROPERTY(QString url READ url NOTIFY urlChanged)

public:
    explicit RendererMetadata(const QString &context, QObject *parent = 0);
    ~RendererMetadata();

    int bitrate() const;
    int duration() const;
    int mediaType() const;
    int sampleRate() const;
    bool seekable() const;
    int videoWidth() const;
    int videoHeight() const;
    int videoPixelWidth() const;
    int videoPixelHeight() const;
    QString url() const;

Q_SIGNALS:
    void bitrateChanged(int);
    void durationChanged(int);
    void mediaTypeChanged(int);
    void sampleRateChanged(int);
    void seekableChanged(bool);
    void videoWidthChanged(int);
    void videoHeightChanged(int);
    void videoPixelWidthChanged(int);
    void videoPixelHeightChanged(int);
    void urlChanged(const QString &url);

    /**
     * emitted after updating all properties
     */
    void changed();

private Q_SLOTS:
    void attributesChanged(const QPps::Changeset &changes);

private:
    QPps::Object m_ppsMetadata;
};

}

#endif
