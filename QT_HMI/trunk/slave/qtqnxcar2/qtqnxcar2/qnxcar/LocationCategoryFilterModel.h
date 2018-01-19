#ifndef QTQNXCAR2_LOCATIONCATEGORYFILTERMODEL_H
#define QTQNXCAR2_LOCATIONCATEGORYFILTERMODEL_H

#include "qtqnxcar2_export.h"

#include <QSortFilterProxyModel>

namespace QnxCar {

/**
 * Model to filter the contents of a LocationCategoryModel
 *
 * When a #filterCategory is set, the model will only contain categories of the given type.
 */
class QTQNXCAR2_EXPORT LocationCategoryFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    /**
     * The category identifier to filter for
     *
     * @accessors filterCategory(), setFilterCategory()
     * @sa LocationCategoryModel::ParentIdRole
     */
    Q_PROPERTY(int filterCategory READ filterCategory WRITE setFilterCategory)

    /**
     * Number of rows in the model
     *
     * Basically rowCount() as a property
     *
     * @accessors categoryCount()
     */
    Q_PROPERTY(int categoryCount READ categoryCount)

public:
    /**
     * Creates an empty model
     * @param parent the QObject parent
     */
    explicit LocationCategoryFilterModel(QObject *parent = 0);

    /// Destroys the instance
    ~LocationCategoryFilterModel();

    /// @sa #filterCategory
    int filterCategory() const;
    /// @sa #filterCategory
    void setFilterCategory(int filterCategory);

    /// @sa #categoryCount
    int categoryCount() const;

private:
    class Private;
    Private *const d;
};

}

#endif
