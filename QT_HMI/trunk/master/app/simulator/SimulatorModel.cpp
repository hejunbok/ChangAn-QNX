#include "SimulatorModel.h"

#include <qpps/object.h>
#include <qpps/simulator.h>

namespace PpsTreeModel {

    struct Node
    {
        enum Type {
            UnknownType,
            /// List of key-value pair
            EntryType,
            /// PPS object
            ObjectType
        };

        Node(const QString &_name = QString())
            : type(UnknownType), name(_name), parent(0) {}
        Node(Type _type, const QString &_name, struct Node *_parent = 0)
        : type(_type), name(_name), parent(_parent) {}
        ~Node()
        {
            qDeleteAll(children);
        }

        Node *findChildByName(const QString &name) const
        {
            Q_FOREACH (Node *node, children) {
                if (node->name == name)
                    return node;
            }
            return 0;
        }

        Type type;
        QString name;
        struct Node *parent;

        QVector<Node *> children;
    };

    struct EntryNode : public Node
    {
        explicit EntryNode(const QString &_name, struct Node *_parent = 0)
            : Node(EntryType, _name, _parent) {}

        QVariant value;
        QByteArray encoding;
    };

    struct ObjectNode : public Node
    {
        explicit ObjectNode(const QString &_name, struct Node *_parent = 0)
            : Node(ObjectType, _name, _parent)  {}
    };

}

using namespace PpsTreeModel;

SimulatorModel::SimulatorModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_root(new Node)
{
    connect(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)),
            this, SLOT(ppsAttributeChanged(QString,QString,QByteArray,QByteArray)));
    connect(QPps::Simulator::self(), SIGNAL(attributeRemoved(QString,QString)),
            this, SLOT(ppsAttributeRemoved(QString,QString)));

    refresh();
}

SimulatorModel::~SimulatorModel()
{
    delete m_root;
}

int SimulatorModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_root->children.size();
    } else {
        Node *node = static_cast<Node *>(parent.internalPointer());
        return node->children.size();
    }
}

int SimulatorModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex SimulatorModel::index(int row, int column, const QModelIndex &parent) const
{
    Node *parentNode = (parent.isValid() ?
        static_cast<Node *>(parent.internalPointer()) : m_root);
    Q_ASSERT(parentNode);
    if (row < 0 || column < 0 || row >= parentNode->children.size()  || column >= columnCount()) {
        return QModelIndex();
    }
    return createIndex(row, column, parentNode->children.at(row));
}

QModelIndex SimulatorModel::parent(const QModelIndex &child) const
{
    Node* node = static_cast<Node *>(child.internalPointer());
    return indexForNode(node->parent);
}

QModelIndex SimulatorModel::indexForNode(Node *node) const
{
    if (!node || !node->parent || node == m_root) {
        return QModelIndex();
    }

    const QModelIndex parentIndex = indexForNode(node->parent);
    Q_ASSERT(node->parent);
    const int row = node->parent->children.indexOf(node);
    Q_ASSERT(row != -1);
    return index(row, 0, parentIndex);
}

ObjectNode *SimulatorModel::createOrGetObject(const QString &path)
{
    Node *node = m_root->findChildByName(path);
    if (!node) {
        beginInsertRows(indexForNode(m_root), m_root->children.size(), m_root->children.size());
        node = new ObjectNode(path, m_root);
        m_root->children.append(node);
        endInsertRows();
    }
    Q_ASSERT(node && node->type == Node::ObjectType);
    ObjectNode *object = static_cast<ObjectNode *>(node);
    return object;
}

EntryNode *SimulatorModel::createOrGetEntry(ObjectNode *object, const QString &key)
{
    Node *node = object->findChildByName(key);
    if (!node) {
        beginInsertRows(indexForNode(object), object->children.size(), object->children.size());
        node = new EntryNode(key, object);
        object->children.append(node);
        endInsertRows();
    }
    Q_ASSERT(node && node->type == Node::EntryType);
    EntryNode *entry = static_cast<EntryNode *>(node);
    return entry;
}

void SimulatorModel::ppsAttributeChanged(const QString &objectPath, const QString &key,
                                         const QByteArray &value, const QByteArray &encoding)
{
    ObjectNode *object = createOrGetObject(objectPath);
    Q_ASSERT(object);
    EntryNode *entry = createOrGetEntry(object, key);
    Q_ASSERT(entry);

    // update entry
    entry->value = value;
    entry->encoding = encoding;

    // notify
    const QModelIndex index = indexForNode(entry);
    Q_ASSERT(index.isValid());
    emit dataChanged(index, index);
}

void SimulatorModel::ppsAttributeRemoved(const QString &objectPath, const QString &key)
{
    ObjectNode *object = createOrGetObject(objectPath);\
    Q_ASSERT(object);
    Node *const entry = object->findChildByName(key);
    if (!entry)
        return;

    const int row = object->children.indexOf(entry);
    Q_ASSERT(row >= 0);
    beginRemoveRows(indexForNode(object), row, row);
    object->children.remove(row);
    delete entry;
    endRemoveRows();
}

QVariant SimulatorModel::data(const QModelIndex &index, int role) const
{
    const Node *node = static_cast<Node *>(index.internalPointer());
    Q_ASSERT(node);
    switch (role) {
    case Qt::DisplayRole: {
        return node->name;
    }
    case KeyRole: {
        if (node->type == Node::EntryType) {
            return node->name;
        }
        break;
    }
    case PpsPathRole: {
        if (node->type == Node::ObjectType) {
            return node->name;
        } else if (node->type == Node::EntryType) {
            return static_cast<const EntryNode *>(node)->parent->name;
        }
        break;
    }
    case EncodingRole:
        if (node->type == Node::EntryType) {
            return static_cast<const EntryNode *>(node)->encoding;
        }
        break;
    case ValueRole:
        if (node->type == Node::EntryType) {
            return static_cast<const EntryNode *>(node)->value;
        }
        break;
    }
    return QVariant();
}

Qt::ItemFlags SimulatorModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    const Node *node = static_cast<Node *>(index.internalPointer());
    if (node->type == Node::EntryType) {
        flags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        flags |= Qt::ItemIsEnabled;
    }
    return flags;
}

void SimulatorModel::refresh()
{
    beginResetModel();

    // clear
    delete m_root;
    m_root = new Node;

    // load
    QMap<QString, QVariantMap> objects = QPps::Simulator::self()->ppsObjects();
    Q_FOREACH (const QString &path, objects.keys()) {
        ObjectNode *object = new ObjectNode(path, m_root);
        m_root->children.append(object);

        Q_FOREACH (const QString &name, objects[path].keys()) {
            EntryNode *entry = new EntryNode(name, object);
            object->children.append(entry);

            entry->value = objects[path][name].toMap()[QStringLiteral("value")];
            entry->encoding = objects[path][name].toMap()[QStringLiteral("encoding")].toByteArray();
        }
    }

    endResetModel();
}
