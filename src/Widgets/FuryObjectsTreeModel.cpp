#include "FuryObjectsTreeModel.h"

#include "FuryObject.h"

#include <QMap>

FuryObjectsTreeModel::FuryObjectsTreeModel(QObject *_parent) :
    QAbstractItemModel(_parent),
    m_root(new ObjectItem(nullptr, nullptr))
{

}

QModelIndex FuryObjectsTreeModel::index(int _row, int _column, const QModelIndex &_parent) const
{
    if (!hasIndex(_row, _column, _parent))
    {
        return QModelIndex();
    }

    if (!_parent.isValid())
    {
        return createIndex(_row, _column, m_root->children[_row]);
    }

    ObjectItem* parentInfo = static_cast<ObjectItem*>(_parent.internalPointer());
    return createIndex(_row, _column, parentInfo->children[_row]);
}

QModelIndex FuryObjectsTreeModel::parent(const QModelIndex &_child) const
{
    if (!_child.isValid())
    {
        return QModelIndex();
    }

    ObjectItem* childInfo = static_cast<ObjectItem*>(_child.internalPointer());
    if (childInfo == nullptr)
    {
        return QModelIndex();
    }

    ObjectItem* parentInfo = childInfo->parent;
    if (parentInfo == nullptr)
    {
        return QModelIndex();
    }

    {
        QVector<ObjectItem*> list = (parentInfo->parent != nullptr) ? parentInfo->parent->children
                                                                    : m_root->children;

        for (int i = 0; i < list.size(); ++i)
        {
            if (list[i] == parentInfo)
            {
                return createIndex(i, 0, list[i]);
            }
        }
    }

    return QModelIndex();
}

int FuryObjectsTreeModel::rowCount(const QModelIndex &_parent) const
{
    if (!_parent.isValid())
    {
        return m_root->children.size();
    }

    ObjectItem* tmp = (static_cast<ObjectItem*>(_parent.internalPointer()));
    return tmp->children.size();
}

int FuryObjectsTreeModel::columnCount(const QModelIndex &_parent) const
{
    Q_UNUSED(_parent);
    return 1;
}

QVariant FuryObjectsTreeModel::data(const QModelIndex &_index, int _role) const
{
    if (_role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (!_index.isValid())
    {
        return QVariant();
    }

    return static_cast<ObjectItem*>(_index.internalPointer())->object->objectName();
}

QVariant FuryObjectsTreeModel::headerData(int _section, Qt::Orientation _orientation,
                                          int _role) const
{
    if (_role == Qt::DisplayRole && _orientation == Qt::Horizontal && _section == 0)
    {
        return "Objects";
    }

    return QVariant();
}

FuryObject *FuryObjectsTreeModel::objectByIndex(const QModelIndex &_index) const
{
    if (_index.isValid())
    {
        ObjectItem* tmp = (static_cast<ObjectItem*>(_index.internalPointer()));
        return tmp->object;
    }

    return nullptr;
}

void FuryObjectsTreeModel::addObjectSlot(FuryObject *_object)
{
    beginResetModel();

    ObjectItem* parentItem = m_root;

    if (_object->parent() != nullptr)
    {
        FuryObject* parentObj = qobject_cast<FuryObject*>(_object->parent());

        if (parentObj != nullptr)
        {
            ObjectItem* newParent = findObject(m_root, parentObj);

            if (newParent != nullptr)
            {
                parentItem = newParent;
            }
        }
    }

    ObjectItem* node = new ObjectItem(parentItem, _object);
    parentItem->children.append(node);

    endResetModel();
}

void FuryObjectsTreeModel::parentChangedSlot(FuryObject *_object)
{
    ObjectItem* item = findObject(m_root, _object);
    if (item == nullptr)
    {
        qDebug() << "not found";
        return;
    }
    ObjectItem* newParent = findObject(m_root, qobject_cast<FuryObject*>(_object->parent()));
    if (newParent == nullptr)
    {
        qDebug() << "parent not found";
        return;
    }

    beginResetModel();

    ObjectItem* oldParent = item->parent;
    oldParent->children.removeOne(item);
    newParent->children.append(item);
    item->parent = newParent;

    endResetModel();
}

FuryObjectsTreeModel::ObjectItem *FuryObjectsTreeModel::findObject(ObjectItem *_where,
                                                                   FuryObject *_object)
{
    if (_where->object == _object)
    {
        return _where;
    }

    for (int i = 0; i < _where->children.size(); ++i)
    {
        ObjectItem* item = findObject(_where->children[i], _object);

        if (item != nullptr)
        {
            return item;
        }
    }

    return nullptr;
}
