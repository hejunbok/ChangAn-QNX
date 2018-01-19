#ifndef QTQNXCAR2_LOCATIONCATEGORYMODEL_H
#define QTQNXCAR2_LOCATIONCATEGORYMODEL_H

#include "qtqnxcar2_export.h"

#include <qnxcar/SqlQueryModel.h>

namespace QnxCar {

/**
 * Model for listing location categories from the navigation database
 *
 * The listed categories are used to group Point-Of-Interest entries.
 */
class QTQNXCAR2_EXPORT LocationCategoryModel : public SqlQueryModel
{
    Q_OBJECT

    Q_ENUMS(Role)
public:
    /// Item roles
    enum Role {
        /// Category ID (int), role name @c "categoryId"
        IdRole = Qt::UserRole + 1,
        /// Category name (QString), role name @c "name"
        NameRole,
        /// Parent category ID (int), role name @c "parentId"
        ParentIdRole,
        /// Category type (QString), role name @c "type"
        TypeRole,
        /// Category type icon Name (QString), role name @c "iconName"
        IconNameRole
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit LocationCategoryModel(QObject *parent = 0);

    /**
     * Maps #Role values to names
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

protected:
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;
    void runQuery() Q_DECL_OVERRIDE;
};

}

#endif
