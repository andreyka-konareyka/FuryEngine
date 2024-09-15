#ifndef FURYTEXTURE_H
#define FURYTEXTURE_H


// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

#include <QString>


//! Класс текстуры
class FuryTexture {
public:
    /*!
    * \brief Конструктор
    * \param[in] _path - Путь к файлу
    */
    FuryTexture(const QString& _path);

    //! Конструктор по умолчанию
    FuryTexture();
    //! Деструктор
    ~FuryTexture();

    /*!
    * \brief Получить идентификатор текстуры в OpenGL
    * \return Возвращает идентификатор текстуры в OpenGL
    */
    inline GLuint idOpenGL() const
    { return m_idOpenGL; }

    /*!
     * \brief Установить идентификатор текстуры в OpenGL
     * \param[in] _idOpenGL - Идентификатор текстуры в OpenGL
     */
    inline void setIdOpenGL(GLuint _idOpenGL)
    { m_idOpenGL = _idOpenGL; }

    /*!
    * \brief Получить путь к файлу
    * \return Возвращает путь к файлу
    */
    inline const QString& path() const
    { return m_path; }

    /*!
    * \brief Получить ширину текстуры
    * \return Возвращает ширину текстуры
    */
    inline int width() const
    { return m_width; }

    /*!
    * \brief Получить высоту текстуры
    * \return Возвращает высоту текстуры
    */
    inline int height() const
    { return m_height; }

    /*!
    * \brief Получить буфер для содержимого текстуры
    * \return Возвращает буфер для содержимого текстуры
    */
    inline unsigned char* data() const
    { return m_data; }

    /*!
    * \brief Получить статус, загружена ли текстура
    * \return Возвращает статус, загружена ли текстура
    */
    inline bool isLoaded() const
    { return m_loaded; }

    /*!
    * \brief Получить статус, готова ли текстура
    * \return Возвращает статус, готова ли текстура
    */
    inline bool isReady() const
    { return m_ready; }

    /*!
    * \brief Установить ширину текстуры
    * \param[in] _width - Ширина текстуры
    */
    inline void setWidth(int _width)
    { m_width = _width; }

    /*!
    * \brief Установить высоту текстуры
    * \param[in] _height - Высота текстуры
    */
    inline void setHeight(int _height)
    { m_height = _height; }

    /*!
    * \brief Установить буфер содержимого текстуры
    * \param[in] _data - Буфер содержимого текстуры
    */
    inline void setData(unsigned char* _data)
    { m_data = _data; }

    /*!
    * \brief Установить статус, загружена ли текстура
    * \param[in] _loaded - Статус, загружена ли текстура
    */
    inline void setLoaded(bool _loaded = true)
    { m_loaded = _loaded; }

    /*!
    * \brief Установить статус, готова ли текстура
    * \return Возвращает статус, готова ли текстура
    */
    inline void setReady(bool _ready = true)
    { m_ready = _ready; }

private:
    //! Идентификатор текстуры в OpenGL
    GLuint m_idOpenGL;
    //! Путь к файлу
    QString m_path;
    //! Буфер для содержимого текстуры
    unsigned char* m_data;
    //! Ширина текстуры
    int m_width;
    //! Высота текстуры
    int m_height;

    //! Готова ли текстура
    bool m_ready;
    //! Загружена ли текстура в буфер
    bool m_loaded;
};

#endif // FURYTEXTURE_H
