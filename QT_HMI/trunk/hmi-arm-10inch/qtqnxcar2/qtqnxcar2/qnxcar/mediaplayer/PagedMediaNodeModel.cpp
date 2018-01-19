#include "PagedMediaNodeModel.h"

#include <math.h>

namespace QnxCar {
namespace MediaPlayer {

PagedMediaNodeModel::PagedMediaNodeModel(QPlayer::QPlayer *qPlayer, QObject *parent)
    : MediaNodeModel(qPlayer, parent)
    , m_pageSize(20)
{
}

PagedMediaNodeModel::~PagedMediaNodeModel()
{
}

QVariant PagedMediaNodeModel::data(const QModelIndex &index, int role) const
{
    int currentPage = floor(index.row() / pageSize()) + 1;
    int nextPage = currentPage + 1;
    int previousPage = (1 > currentPage - 1 ? 1 : currentPage - 1);

    // Load the current page if it doesn't already exist (it should)
    loadPage(currentPage);

    // Load the previous page if we happened to arrive somewhere in the middle of a list
    loadPage(previousPage);

    // Pre-fetch the next page
    loadPage(nextPage);

    return MediaNodeModel::data(index, role);
}

int PagedMediaNodeModel::pageSize() const
{
    return m_pageSize;
}

void PagedMediaNodeModel::setPageSize(int size)
{
    m_pageSize = size;
}

void PagedMediaNodeModel::loadPage(int page) const
{
    if(!m_loadedPages.contains(page)) {
        // Add the page number to known list of loaded pages so we don't try to load the same page again
        m_loadedPages.append(page);

        // Load the page
        doLoadPage(page);
    }
}

void PagedMediaNodeModel::doLoadPage(int page) const
{
    Q_UNUSED(page);

    qWarning("%s: Method must be overridden in subclass", Q_FUNC_INFO);
}

void PagedMediaNodeModel::insert(int index, QList<MediaNode> mediaNodes)
{
    int i = 0;
    for(i=mediaNodes.length() - 1; i >= 0 ;i--)
    {
        QString name = mediaNodes[i].name;
        QString title = mediaNodes[i].metadata.title;
        if(!title.isEmpty())
        {
            name = title;
        }

        if(name.contains(".3gp") || name.contains(".mpeg") || name.contains(".avi") || name.contains(".MPEG") || name.contains(".3GP") || name.contains(".AVI"))
        {
            mediaNodes.removeAt(i);
        }
    }

    // Create indicies for the added nodes as we append them
    if(mediaNodes.length() > 0) {
        beginInsertRows(QModelIndex(), index, index + mediaNodes.length() - 1);
        MediaNodeModel::insert(index, mediaNodes);
        endInsertRows();
    }
}

void PagedMediaNodeModel::clear()
{
    // Clear the known loaded pages
    m_loadedPages.clear();

    // Clear the nodes
    MediaNodeModel::clear();
}

}
}
