#ifndef QTQNXCAR2_MODELMONITOR_H
#define QTQNXCAR2_MODELMONITOR_H

#include <QObject>

#include "qtqnxcar2_export.h"

class QAbstractItemModel;

namespace QnxCar {

/**
 * ModelMonitor for QML
 *
 * It only makes sense to use flat QAIMs at the moment
 */
class QTQNXCAR2_EXPORT ModelMonitor : public QObject
{
    Q_OBJECT

    /// Property wrapping QAIM::rowCount()
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)

public:
    explicit ModelMonitor(QObject *parent = 0);
    ~ModelMonitor();

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    int count() const;

Q_SIGNALS:
    /// Triggered when the rowCount() of the model changed
    void countChanged();

    /// Triggered when the model was changed
    void modelChanged();

private Q_SLOTS:
    void handleDeleted();

    void updateCount();

private:
    void setCount(int count);

private:
    class Private;
    Private *const d;
};

}

#endif
