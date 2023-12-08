#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H


#include "Texture.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <QMap>
#include <QString>
#include <QQueue>


//! Класс менеджера текстур
class TextureManager
{
public:
    //! Конструктор
    TextureManager();
    //! Деструктор
    ~TextureManager();

    /*!
    * \brief Добавление текстуры в очередь загрузки.
    * \param[in] _path - Путь к файлу текстуры
    * \param[in] _name - Название текстуры (псевдоним)
    */
    void addTexture(const QString& _path, const QString& _name = "");

    /*!
    * \brief Получить текстуру по названию
    * \param[in] _name - Название текстуры
    * \return Возвращает текстуру по наименованию. Если её нет, то пустую текстуру
    */
    const Texture& textureByName(const QString &_name) const;

    /*!
    * \brief Получить текстуру по пути к файлу текстуры
    * \param[in] _path - Путь к файлу текстуры
    * \return Возвращает текстуру по пути к файлу текстуры. Если её нет, то пустую текстуру
    */
    const Texture& textureByPath(const QString &_path) const;

    //! Загрузить часть текстур
    void loadTexturePart();

    //! Завершение потока загрузки текстур и ожидание
    void stopLoopAndWait();

private:
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
    QMap<QString, Texture*> m_textures;
    //! Отображение: Наименование текстуры -> Путь к файлу текстуры
    QMap<QString, QString> m_nameToPath;

    //! Очередь текстур на загрузку из файла
    QQueue<Texture*> m_textureLoadQueue;
    //! Очередь текстур на связывание с OpenGL
    QQueue<Texture*> m_textureBindQueue;

    //! Пустая текстура по умолчанию
    Texture m_emptyTexture;
};



#endif // TEXTUREMANAGER_H
