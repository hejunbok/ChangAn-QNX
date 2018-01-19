#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include <QAbstractItemModel>

namespace PpsTreeModel {
struct Node;
struct ObjectNode;
struct EntryNode;
}

class SimulatorModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Role {
        /// Return PPS path in case index is either an object or an entry
        PpsPathRole = Qt::UserRole + 1,
        /// Return PPS key in case the index is an entry
        KeyRole,
        /// Return PPS attribute value in case index is an entry
        ValueRole,
        /// Return PPS attribute encoding in case index is an entry
        EncodingRole,
    };

    explicit SimulatorModel(QObject* parent = 0);
    virtual ~SimulatorModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

private Q_SLOTS:
    void ppsAttributeChanged(const QString &objectPath, const QString &key,
                             const QByteArray &value, const QByteArray &encoding);
    void ppsAttributeRemoved(const QString &objectPath, const QString &key);

private:
    /// Reload the model
    void refresh();

    QModelIndex indexForNode(PpsTreeModel::Node *parent) const;

    PpsTreeModel::ObjectNode *createOrGetObject(const QString &path);
    PpsTreeModel::EntryNode *createOrGetEntry(PpsTreeModel::ObjectNode *object, const QString &key);

    /**
     * Root of the PPS tree model (always non-null)
     *
     * The tree looks like:
     * - root (type Node::UnknownType)
     *   - object1 (type Node::ObjectType)
     *     - key11 (type Node::EntryType)
     *     - key12
     *   - object2
     *     - key21
     */
    PpsTreeModel::Node *m_root;
};

#endif
