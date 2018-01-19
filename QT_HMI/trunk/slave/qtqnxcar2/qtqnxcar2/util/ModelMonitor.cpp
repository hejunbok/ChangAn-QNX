#include "ModelMonitor.h"

#include <QAbstractItemModel>

namespace QnxCar {

class ModelMonitor::Private
{
public:
    Private()
        : model(0)
        , count(0)
    {}

    QAbstractItemModel* model;
    int count;
};

ModelMonitor::ModelMonitor(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

ModelMonitor::~ModelMonitor()
{
    delete d;
}

QAbstractItemModel* ModelMonitor::model() const
{
    return d->model;
}

void ModelMonitor::setModel(QAbstractItemModel* model)
{
    if (d->model == model)
        return;

    if (d->model) {
        disconnect(d->model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateCount()));
        disconnect(d->model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateCount()));
        disconnect(d->model, SIGNAL(modelReset()), this, SLOT(updateCount()));
        disconnect(d->model, SIGNAL(destroyed(QObject*)), this, SLOT(handleDeleted()));
    }

    d->model = model;

    if (d->model) {
        connect(d->model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateCount()));
        connect(d->model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateCount()));
        connect(d->model, SIGNAL(modelReset()), this, SLOT(updateCount()));
        connect(d->model, SIGNAL(destroyed(QObject*)), this, SLOT(handleDeleted()));
    }
    emit modelChanged();

    // also update count
    updateCount();
}

void ModelMonitor::handleDeleted()
{
    d->model = 0;
    emit modelChanged();
    updateCount();
}

int ModelMonitor::count() const
{
    return d->count;
}

void ModelMonitor::setCount(int count)
{
    if (d->count == count)
        return;

    d->count = count;
    emit countChanged();
}

void ModelMonitor::updateCount()
{
    setCount(d->model ? d->model->rowCount() : 0);
}

}
