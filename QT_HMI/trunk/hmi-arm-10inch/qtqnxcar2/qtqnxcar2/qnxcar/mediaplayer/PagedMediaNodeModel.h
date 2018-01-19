#ifndef PAGEDMEDIANODEMODEL_H
#define PAGEDMEDIANODEMODEL_H

#include "qtqnxcar2_export.h"

#include "MediaNodeModel.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief An abstract model used for media node lists with an unknown length, which can be loaded in a linear-paged
 * fashion.
 */
class QTQNXCAR2_EXPORT PagedMediaNodeModel : public MediaNodeModel
{
    Q_OBJECT

public:
    PagedMediaNodeModel(QPlayer::QPlayer *qPlayer, QObject *parent = 0);
    virtual ~PagedMediaNodeModel();

    /**
     * MediaNodeModel reimplemented method. Determines the page of the item from which data is being requested,
     * and ensures that page, as well as the surrounding pages of nodes are loaded, then returns the requested role data
     * for the node at the specified index.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * @brief Returns the configured page size for this model.
     * @return The page size.
     */
    int pageSize() const;

    /**
     * @brief Sets the page size for this model. All MediaNodeModel instances are created with a default page size
     * of 20.
     * @param size The page size.
     */
    void setPageSize(int size);

    /**
    * @brief Checks if the requested page has already been loaded, and if not, loads it and keeps a record, otherwise
    * does nothing.
    * @param page The number of the page to load.
    */
    void loadPage(int page) const;

    /**
     * @brief The abstract doLoadPage method is called when the PagedMediaNodeModel is attempting to load additional
     * pages of data. This method must be overridden in subclasses but NOT called directly. If a subclass needs to
     * load data, it should use the @c loadPage method so that a record of the request can be tracked.
     * @param page The number of the page of nodes to load.
     */
    virtual void doLoadPage(int page) const;

protected:
    /**
     * @brief Inserts media nodes to the model storage at the index specified, and emits the necessary signals to notify
     * clients of the added data.
     * @param index The index at which the nodes should be appended.
     * @param mediaNodes The media nodes to add.
     */
    void insert(int index, QList<MediaNode> mediaNodes) Q_DECL_OVERRIDE;

    /**
     * @brief Clears the current media node storage and emits the necessary signals to notify clients
     * of the removed data. This specialized override also clears the known loaded page data.
     */
    void clear() Q_DECL_OVERRIDE;

private:
    /**
     * @brief The number of items in each page.
     */
    int m_pageSize;

    /**
     * @brief A list of known loaded pages, used to prevent pages from being loaded more than once.
     */
    mutable QVector<int> m_loadedPages;
};

}
}

#endif // PAGEDMEDIANODEMODEL_H
