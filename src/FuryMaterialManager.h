#ifndef FURYMATERIALMANAGER_H
#define FURYMATERIALMANAGER_H

#include <QMap>
#include <QString>

class FuryMaterial;
class FuryPbrMaterial;


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
     */
    static FuryMaterialManager* createInstance();
    //! Удаление экземпляра класса
    static void deleteInstance();

    /*!
     * \brief Создание материала
     * \param[in] _name - Название
     * \return Возвращает материал
     */
    FuryMaterial* createMaterial(const QString& _name);

    /*!
     * \brief Создание PBR материала
     * \param[in] _name - Название
     * \return Возвращает PBR материал
     */
    FuryPbrMaterial* createPbrMaterial(const QString& _name);

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

    //! Сохранение материалов
    void saveMaterials();

private:
    //! Конструктор
    FuryMaterialManager();
    //! Деструктор
    ~FuryMaterialManager();
    //! Экземпляр класса
    static FuryMaterialManager* s_instance;

private:
    //! Отображение: Название материала -> Материал
    QMap<QString, FuryMaterial*> m_materials;
    //! Материал по умолчанию
    FuryMaterial* m_defaultMaterial;
};

#endif // FURYMATERIALMANAGER_H
