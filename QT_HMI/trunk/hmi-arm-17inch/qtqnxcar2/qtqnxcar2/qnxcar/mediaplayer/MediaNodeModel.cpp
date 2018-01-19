#include "MediaNodeModel.h"

#include <QDebug>

#include "qnxcar/mediaplayer/TypeConverter.h"
#include "iostream"

namespace QnxCar {
namespace MediaPlayer {

MediaNodeModel::MediaNodeModel(QPlayer::QPlayer *qPlayer, QObject *parent)
    : QAbstractListModel(parent)
    , m_qPlayer(qPlayer)
{
}

MediaNodeModel::~MediaNodeModel()
{
}

int MediaNodeModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_mediaNodes.size();
}

QVariant MediaNodeModel::data(const QModelIndex &index, int role) const
{
    MediaNode mediaNode;
    QMap<int, MediaNode>::const_iterator i;

    i = m_mediaNodes.constFind(index.row());
    if(i != m_mediaNodes.constEnd()) {
        mediaNode = i.value();
    }

//    QString name = mediaNode.name;
//    QString title = mediaNode.metadata.title;
//    if(!title.isEmpty())
//    {
//        name = title;
//    }

//    if(name.contains(".3gp") || name.contains(".mpeg") || name.contains(".avi") || name.contains(".MPEG") || name.contains(".3GP") || name.contains(".AVI"))
//    {
//        return QVariant();
//    }

    switch (role) {
        case IdRole:
            return mediaNode.id;
        case MediaSourceIdRole:
            return mediaNode.mediaSourceId;
        case Qt::DisplayRole:
        case NameRole: {
            QString name;
            if(!mediaNode.metadata.title.isEmpty()) {
                name = mediaNode.metadata.title;
            } else {
                name = mediaNode.name;
            }
            return name.toUtf8().data();
        }
        case TypeRole:
            return mediaNode.type;
        case ArtworkRole:
            return mediaNode.metadata.artwork;
        case FolderTypeRole:
            return mediaNode.getExtendedMetadata(QLatin1String("folder_type"));
        case ReadyRole:
            return mediaNode.ready;
    }

    return QVariant();
}

QHash<int,QByteArray> MediaNodeModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();

    names.insert(IdRole, "id");
    names.insert(MediaSourceIdRole, "mediaSourceId");
    names.insert(NameRole, "name");
    names.insert(TypeRole, "type");
    names.insert(ArtworkRole, "artwork");
    names.insert(FolderTypeRole, "folderType");
    names.insert(ReadyRole, "ready");

    return names;
}

bool MediaNodeModel::nodeExists(int index) const
{
    return m_mediaNodes.contains(index);
}

void MediaNodeModel::insert(int index, QList<MediaNode> mediaNodes)
{
    if(mediaNodes.length() > 0) {
        // Append nodes to those already in storage
        for(int i = 0; i < mediaNodes.length(); i++) {
            m_mediaNodes.insert(index + i, mediaNodes[i]);
        }

        // TODO: We should only be loading metadata for nodes which are being displayed.
        // The challenge with this is that our only indication that nodes are being displayed is the invocation of
        // the data() method. We would need to avoid sending off duplicate metadata commands whilst waiting for them
        // to return, so we'll need a way of checking if there's already one in progress for the node in question.

        // Load metadata and extended metadata for the added nodes
        for(QList<MediaNode>::const_iterator i = mediaNodes.constBegin(); i != mediaNodes.constEnd(); i++) {
            MediaNode mediaNode = *i;

            if(mediaNode.type == MediaNodeType::AUDIO) {
                QPlayer::MetadataCommand *mdCommand = new QPlayer::MetadataCommand(mediaNode.mediaSourceId, mediaNode.id);
                connect(mdCommand, &QPlayer::MetadataCommand::complete, this, &MediaNodeModel::onMetadataCommandComplete);
                m_qPlayer->getMetadata(mdCommand);
            } else if(mediaNode.type == MediaNodeType::FOLDER && !mediaNode.hasExtendedMetadata((QStringLiteral("folder_type")))) {
                QPlayer::ExtendedMetadataCommand *emdCommand = new QPlayer::ExtendedMetadataCommand(mediaNode.mediaSourceId, mediaNode.id, QStringList(QLatin1String("folder_type")));
                connect(emdCommand, &QPlayer::ExtendedMetadataCommand::complete, this, &MediaNodeModel::onExtendedMetadataCommandComplete);

                m_qPlayer->getExtendedMetadata(emdCommand);
            }
        }
    }
}

void MediaNodeModel::clear()
{
    // Clear existing nodes
    beginRemoveRows(QModelIndex(), 0, rowCount());
    m_mediaNodes.clear();
    endRemoveRows();
}

void MediaNodeModel::onMetadataCommandComplete(QPlayer::MetadataCommand *command)
{
    // Find the media node in storage
    int index = findMediaNodeIndex(command->mediaSourceId(), command->mediaNodeId());

    if(index != -1) {
        // Convert the metadata
        Metadata metadata = TypeConverter::convertMetadata(command->result());

        // Update the node's metadata and notify of the changed data
        m_mediaNodes.find(index).value().metadata = metadata;

        emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    } else {
        qDebug() << Q_FUNC_INFO << "Unable to find media node with ID" << command->mediaNodeId();
    }
}

void MediaNodeModel::onExtendedMetadataCommandComplete(QPlayer::ExtendedMetadataCommand *command)
{
    // Find the media node in storage
    int index = findMediaNodeIndex(command->mediaSourceId(), command->mediaNodeId());

    if(index != -1) {
        QStringList keys = command->result().keys();
        QMap<int, MediaNode>::iterator mn = m_mediaNodes.find(index);
        for(int i = 0; i < keys.size(); i++) {
            mn.value().setExtendedMetadata(keys[i], command->result().value(keys[i]));
        }

        // Notify of the changed data
        emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    } else {
        qDebug() << Q_FUNC_INFO << "Unable to find media node with ID" << command->mediaNodeId();
    }
}

int MediaNodeModel::findMediaNodeIndex(const int mediaSourceId, const QString mediaNodeId)
{
    int index = -1;

    for(QMap<int, MediaNode>::const_iterator i = m_mediaNodes.constBegin(); i != m_mediaNodes.constEnd(); i++) {
        if(i.value().mediaSourceId == mediaSourceId && i.value().id == mediaNodeId) {
            index = i.key();
            break;
        }
    }

    return index;
}

}
}
