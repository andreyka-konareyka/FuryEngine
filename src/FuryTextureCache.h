#ifndef FURYTEXTURECACHE_H
#define FURYTEXTURECACHE_H

#include <QObject>


class FuryTexture;


//! Кэш текстуры
class FuryTextureCache : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _textureName - Название текстуры
     */
    FuryTextureCache(const QString& _textureName = QString());

    /*!
     * \brief Установка названия текстуры
     * \param[in] _textureName - Название текстуры
     */
    void setTextureName(const QString& _textureName);

    /*!
     * \brief Текстура
     * \return Возвращает текстуру
     */
    const FuryTexture& texture() const;

private slots:
    /*!
     * \brief Слот перезапроса текстуры у менеджера
     * \param[in] _textureName - Название текстуры
     */
    void requestTextureSlot(const QString& _textureName);

private:
    //! Инициализация соединений сигналов и слотов
    void initConnections();

private:
    //! Название текстуры
    QString m_textureName;
    //! Текстура
    const FuryTexture* m_texture;
};

#endif // FURYTEXTURECACHE_H
