#ifndef QTQNXCAR2_MESSAGESFILTERMODEL_H
#define QTQNXCAR2_MESSAGESFILTERMODEL_H

#include "qtqnxcar2_export.h"

#include "MessagesModel.h"

#include <QSortFilterProxyModel>

namespace QnxCar {

/**
 * Filter model to filter messages from MessagesModel by type.
 * When using different views on the same message data, using a single
 * MessagesModel with different filters on top of a single unfiltered messages model
 * is a more convenient and performant approach than using multiple MessagesModel
 * instances duplicating data.
 */
class QTQNXCAR2_EXPORT MessagesFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MessagesFilterModel(QObject *parent = 0);

    ~MessagesFilterModel();

    /**
     * Reduce model contents to messages of a given @p type.
     * Removes any previously set filter.
     *
     * @param type a #MessagesModel::MessageType
     * @sa clearFilters(), addFilterMessageType()
     */
    Q_INVOKABLE void setFilterMessageType(int type);

    /**
     * Adds @p type to the list of shown types.
     * Does not clear previously set types.
     *
     * @param type a #MessagesModel::MessageType
     * @sa clearFilters(), setFilterMessageType()
     */
    Q_INVOKABLE void addFilterMessageType(int type);

    /**
     * Clears filtering set by setFilterMessageType(), addFilterMessageType()
     */
    Q_INVOKABLE void clearFilters();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    class Private;
    Private *const d;
};

}

#endif
