#ifndef QTQNXCAR2_APPLICATIONFILTERMODEL_H
#define QTQNXCAR2_APPLICATIONFILTERMODEL_H

#include "qtqnxcar2_export.h"

#include <QSortFilterProxyModel>
#include <QStringList>

namespace QnxCar {

/**
 * Model for filtering application data information based on application groups
 *
 * To be used on top of ApplicationListModel.
 */
class QTQNXCAR2_EXPORT ApplicationFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    /**
     * Application group name that application data objects need to have
     * to be part of the proxy's content
     *
     * @note takes precedence over blacklisted groups
     *
     * @accessors group(), setGroup()
     * @sa setBlacklist(), ApplicationData::group(), ApplicationListModel::GroupRole
     */
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)

public:
    /**
     * Creates an empty proxy without any filters set.
     *
     * @param parent the QObject model
     */
    explicit ApplicationFilterModel(QObject *parent = 0);
    ~ApplicationFilterModel();

    /// #group
    QString group() const;

    /// #group
    void setGroup(const QString &group);

    /**
     * Sets a list of groups that will be always
     * filtered out even if no group is set. If @c group is explicitly set
     * to a group in the blacklist, the group will be not filtered out.
     *
     * @param groups list of groups to hide
     */
    void setBlacklist(const QStringList &groups);

protected:
    /**
     * Accepts application data that either has the desired #group or, if #group is empty, is not
     * in the list of blacklisted groups
     *
     * @param source_row row in the source model
     * @param source_parent parent in the source model
     * @return @c true if the row should be part of the proxy, otherwise @c false
     */
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

Q_SIGNALS:
    void groupChanged(const QString &group);

private:
    class Private;
    Private *const d;
};

}

#endif
