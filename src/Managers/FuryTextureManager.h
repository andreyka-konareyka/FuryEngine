#ifndef FURYTEXTUREMANAGER_H
#define FURYTEXTUREMANAGER_H


#include "FuryTexture.h"

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

#include <QMap>
#include <QString>
#include <QQueue>
#include <QMutex>
#include <QThread>


//! Класс менеджера текстур
class FuryTextureManager : public QThread
{
    Q_OBJECT

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

    /*!
     * \brief Получение всех наименований текстур
     * \return Возвращает список имен текстур
     */
    QStringList allTextureNames() const;

    /*!
     * \brief Получение пустой текстуры
     * \return Возвращает пустую текстуру
     */
    inline const FuryTexture& emptyTexture() const
    { return m_emptyTexture; }

signals:
    /*!
     * \brief Сигнал добавления текстуры
     * \param[in] _texture - Текстура
     */
    void addTextureSignal(const QString& _textureName);

    /*!
     * \brief Сигнал изменения текстуры
     * \param[in] _texture - Текстура
     */
    void editTextureSignal(const QString& _textureName);

    /*!
     * \brief Сигнал удаления текстуры
     * \param[in] _texture - Текстура
     */
    void deleteTextureSignal(const QString& _textureName);

    //! Сигнал, что какие-то текстуры готовы к подсоединению
    void texturesReadyToBindSignal();

private:
    //! Конструктор
    FuryTextureManager();
    //! Деструктор
    ~FuryTextureManager();
    //! Экземпляр класса
    static FuryTextureManager* s_instance;

    //! Запрещаем конструктор копирования
    FuryTextureManager(const FuryTextureManager&) = delete;
    //! Запрещаем оператор присваивания
    FuryTextureManager& operator=(const FuryTextureManager&) = delete;


    /*!
    * \brief Бесконечный цикл подгрузки текстур с диска.
    * Используется в отдельном потоке
    */
    void run() override;

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

    //! Мьютекс для очереди загрузки
    mutable QMutex m_loadMutex;
    //! Мьютекс для очереди связывания с OpenGL
    mutable QMutex m_bindMutex;
    //! Мьютекс для m_nameToPath
    mutable QMutex m_nameMutex;
    //! Мьютекс для m_textures
    mutable QMutex m_textureMutex;

    //! Пустая текстура по умолчанию
    FuryTexture m_emptyTexture;
};



#endif // FURYTEXTUREMANAGER_H
