#ifndef FURYMATERIALMANAGER_H
#define FURYMATERIALMANAGER_H

#include "Logger/FuryLogger.h"

#include <QMap>
#include <QString>

class FuryMaterial;
class FuryPbrMaterial;
class FuryPhongMaterial;


//! Класс менеджера материалов
class FuryMaterialManager
{
public:
    /*!
     * \brief Получение экземпляра класса
     * \return Возвращает экземпляр класса
     */
    static FuryMaterialManager* instance();

    /*!
     * \brief Создание экземпляра класса
     * \return Возвращает экземпляр класса
     * \throw FuryException - При повторном создании
     */
    static FuryMaterialManager* createInstance();

    /*!
     * \brief Удаление экземпляра класса
     * \throw FuryException - При удалении пустого
     */
    static void deleteInstance();

    /*!
     * \brief Создание Phong материала
     * \param[in] _name - Название
     * \return Возвращает материал
     */
    FuryPhongMaterial* createPhongMaterial(const QString& _name);

    /*!
     * \brief Создание PBR материала
     * \param[in] _name - Название
     * \return Возвращает PBR материал
     */
    FuryPbrMaterial* createPbrMaterial(const QString& _name);

    /*!
     * \brief Шаблонное создание любого материала, наследника FuryMaterial
     * \param[in] _name - Название
     * \return Возвращает созданный материал
     */
    template <typename T>
    T* createUserMaterial(const QString& _name);

    /*!
     * \brief Получение материала по названию
     * \param[in] _name - Название
     * \return Возвращает материал
     */
    FuryMaterial* materialByName(const QString& _name);

    /*!
     * \brief Удаление материала
     * \param[in] _name - Название
     */
    void deleteMaterial(const QString& _name);

    /*!
     * \brief Проверка существования материала
     * \param[in] _name - Название
     * \return Возвращает признак существования такого материала
     */
    bool materialExist(const QString& _name);

    /*!
     * \brief Получение названий всех материалов
     * \return Возвращает список наименований
     */
    QList<QString> allMaterialNames() const;

    /*!
     * \brief Попытка загрузить материал из JSON
     * \param[in] _name - Название материала
     * \return Возвращает признак успеха.
     */
    bool tryLoadMaterial(const QString& _name);

    //! Сохранение материалов
    void saveMaterials();

private:
    //! Конструктор
    FuryMaterialManager();
    //! Деструктор
    ~FuryMaterialManager();
    //! Экземпляр класса
    static FuryMaterialManager* s_instance;

    //! Запрещаем конструктор копирования
    FuryMaterialManager(const FuryMaterialManager&) = delete;
    //! Запрещаем оператор присваивания
    FuryMaterialManager& operator=(const FuryMaterialManager&) = delete;

private:
    //! Отображение: Название материала -> Материал
    QMap<QString, FuryMaterial*> m_materials;
    //! Материал по умолчанию
    FuryMaterial* m_defaultMaterial;
};




template<typename T>
T *FuryMaterialManager::createUserMaterial(const QString &_name)
{
    if (m_materials.contains(_name))
    {
        Debug(ru("[ ВНИМАНИЕ ] Материал (%1) уже существовал.").arg(_name));
        return static_cast<T*>(m_materials.value(_name));
    }

    Debug(ru("Создан материал: (%1)").arg(_name));

    T* material = new T;
    m_materials.insert(_name, material);
    return material;
}

#endif // FURYMATERIALMANAGER_H
