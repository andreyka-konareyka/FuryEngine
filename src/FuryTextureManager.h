#ifndef FURYTEXTUREMANAGER_H
#define FURYTEXTUREMANAGER_H


#include "FuryTexture.h"

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

#include <QMap>
#include <QString>
#include <QQueue>


//! Класс менеджера текстур
class FuryTextureManager
{
public:
    /*!
     * \brief Получить экземпляра класса
     * \return Возвращает экземпляр класса
     */
    static FuryTextureManager* instance();

    /*!
     * \brief Создать экземпляр класса
     * \return Возвращает экземпляр класса
     */
    static FuryTextureManager* createInstance();

    //! Удалить экземпляр класса
    static void deleteInstance();

    /*!
    * \brief Добавление текстуры в очередь загрузки.
    * \param[in] _path - Путь к файлу текстуры
    * \param[in] _name - Название текстуры (псевдоним)
    */
    void addTexture(const QString& _path, const QString& _name = "");

    /*!
     * \brief Добавление текстур из другого потока
    * \param[in] _path - Путь к файлу текстуры
    * \param[in] _name - Название текстуры (псевдоним)
     */
    void addTextureFromAnotherThread(const QString& _path, const QString& _name = "");

    /*!
    * \brief Получить текстуру по названию
    * \param[in] _name - Название текстуры
    * \return Возвращает текстуру по наименованию. Если её нет, то пустую текстуру
    */
    const FuryTexture& textureByName(const QString &_name) const;

    /*!
    * \brief Получить текстуру по пути к файлу текстуры
    * \param[in] _path - Путь к файлу текстуры
    * \return Возвращает текстуру по пути к файлу текстуры. Если её нет, то пустую текстуру
    */
    const FuryTexture& textureByPath(const QString &_path) const;

    /*!
     * \brief Получить путь по названию
     * \param[in] _name - Название
     * \return Возвращает путь
     */
    QString pathByName(const QString& _name) const;

    //! Загрузить часть текстур
    void loadTexturePart();

    //! Завершение потока загрузки текстур и ожидание
    void stopLoopAndWait();

private:
    //! Конструктор
    FuryTextureManager();
    //! Деструктор
    ~FuryTextureManager();
    //! Экземпляр класса
    static FuryTextureManager* s_instance;


    /*!
    * \brief Бесконечный цикл подгрузки текстур с диска.
    * Используется в отдельном потоке
    */
    void infiniteLoop();

    //! Признак, надо ли завершить поток загрузки текстур
    bool m_needStop;
    //! Признак, завершился ли поток загрузки текстур
    bool m_stopped;

private:
    //! Отображение: Путь к файлу текстуры -> Текстура
    QMap<QString, FuryTexture*> m_textures;
    //! Отображение: Наименование текстуры -> Путь к файлу текстуры
    QMap<QString, QString> m_nameToPath;

    //! Очередь текстур на загрузку из файла
    QQueue<FuryTexture*> m_textureLoadQueue;
    //! Очередь текстур на связывание с OpenGL
    QQueue<FuryTexture*> m_textureBindQueue;

    //! Очередь добавления текстур (для другого потока)
    QQueue<QPair<QString, QString>> m_texturesAddQueue;

    //! Пустая текстура по умолчанию
    FuryTexture m_emptyTexture;
};



#endif // FURYTEXTUREMANAGER_H
