#ifndef FURYOBJECTSTREEMODEL_H
#define FURYOBJECTSTREEMODEL_H

#include <QAbstractItemModel>

class FuryObject;

class FuryObjectsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FuryObjectsTreeModel(QObject* _parent = nullptr);

    QModelIndex index(int _row, int _column, const QModelIndex &_parent) const override;
    QModelIndex parent(const QModelIndex &_child) const override;
    int rowCount(const QModelIndex &_parent) const override;
    int columnCount(const QModelIndex &_parent) const override;
    QVariant data(const QModelIndex &_index, int _role) const override;
    QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const override;

    /*!
     * \brief Получить объект по индексу
     * \param[in] _index - Индекс
     * \return Возвращает объект, если он есть. nullptr - иначе.
     */
    FuryObject* objectByIndex(const QModelIndex& _index) const;

public slots:
    void addObjectSlot(FuryObject* _object);
    void parentChangedSlot(FuryObject* _object);

private:
    struct ObjectItem
    {
        ObjectItem* parent;
        FuryObject* object;
        QVector<ObjectItem*> children;

        ObjectItem(ObjectItem* _parent, FuryObject* _object) :
            parent(_parent), object(_object) {}
        ObjectItem() = delete;
    };

    /*!
     * \brief Найти item по объекту
     * \param[in] _where - В каком элементе искать
     * \param[in] _object - Объект
     * \return Возвращает item, если он есть. nullptr - иначе
     */
    ObjectItem* findObject(ObjectItem* _where, FuryObject* _object);

private:
    ObjectItem* m_root;
};

#endif // FURYOBJECTSTREEMODEL_H
