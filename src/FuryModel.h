#ifndef FURYMODEL_H
#define FURYMODEL_H

#include "FuryMesh.h"

#include <QVector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


//! Класс модели
class FuryModel
{
public:
    /*!
     * \brief Конструктор
     * \param[in] _path - Путь до файла модели
     */
    FuryModel(const QString& _path);
    //! Конструктор по умолчанию
    FuryModel();
    //! Деструктор
    ~FuryModel();

    /*!
     * \brief Отрисовка с заданным материалом
     * \param[in] _shader - Шейдер
     * \param[in] _material - Материал. Если nullptr, то берётся материал из меша
     */
    void draw(Shader* _shader, FuryMaterial* _material = nullptr);

    //! Отрисовка для карты теней
    void drawShadowMap();

    inline const QString& path() const
    { return m_path; }

    inline bool isReady() const
    { return m_ready; }

    inline void setReady(bool _ready = true)
    { m_ready = _ready; }

    inline bool isLoaded() const
    { return m_loaded; }

    inline void setLoaded(bool _loaded = true)
    { m_loaded = _loaded; }

    inline float modelRadius()
    {  return m_modelRadius; }

    void setupMesh();

    /*!
     * \brief Загрузить модель из файла
     * \param[in] _path - Путь к модели
     */

    /*!
     * \brief Загрузить модель из файла
     * \return Возвращает, успешно ли загружена модель
     */
    bool loadModel();

private:
    /*!
     * \brief Обработка узла
     * \param[in] _node - Текущий узел
     * \param[in] _scene - Сцена
     */
    void processNode(aiNode* _node, const aiScene* _scene);

    /*!
     * \brief Обработка меша
     * \param[in] _mesh - Текущий меш
     * \param[in] _scene - сцена
     * \return Возвращает fury-меш
     */
    FuryMesh* processMesh(aiMesh* _mesh, const aiScene* _scene);

    /*!
     * \brief Вычисление габаритов всей модели по минимальным и максимальным
     * координанам
     * \param[in] _vertex - Обрабатываемая вершина
     */
    void calculateMinMaxVertex(const glm::vec3& _vertex);

    //! Вычисляет радиус сферы, в которую вписана модель
    void calculateRadius();

private:
    //! Путь к файлу
    QString m_path;
    //! Список мешей
    QVector<FuryMesh*> m_meshes;
    //! Директория
    QString m_directory;

    //! Радиус сферы, в которую вписана модель
    float m_modelRadius;
    //! Минимальная координата куба, в которую вписана модель
    glm::vec3 m_minimumVertex;
    //! Максимальная координата куба, в которую вписана модель
    glm::vec3 m_maximumVertex;

    //! Готова ли модель для использования
    bool m_ready;
    //! Загружена ли модель в память
    bool m_loaded;
};

#endif // FURYMODEL_H
