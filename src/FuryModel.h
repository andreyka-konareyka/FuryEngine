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
     * \param[in] _transformation - Трансформация модели
     * \param[in] _material - Материал. Если nullptr, то берётся материал из меша
     */
    void draw(Shader* _shader, const glm::mat4& _transformation,
              FuryMaterial* _material = nullptr);

    /*!
     * \brief Отрисовка для карты теней
     * \param[in] _shader - Шейдер
     * \param[in] _transformation - Трансформация модели
     */
    void drawShadowMap(Shader* _shader, const glm::mat4& _transformation);

    /*!
     * \brief Получение пути к файлу модели
     * \return Возвращает путь к файлу модели
     */
    inline const QString& path() const
    { return m_path; }

    /*!
     * \brief Получение признак готовности
     * \return Возвращает признак готовности
     */
    inline bool isReady() const
    { return m_ready; }

    /*!
     * \brief Установка признака готовности
     * \param[in] _ready - Признак готовности
     */
    inline void setReady(bool _ready = true)
    { m_ready = _ready; }

    /*!
     * \brief Получение признака загруженности
     * \return Возвращает признак загруженности
     */
    inline bool isLoaded() const
    { return m_loaded; }

    /*!
     * \brief Установка признака загруженности
     * \param[in] _loaded - Признак загруженности
     */
    inline void setLoaded(bool _loaded = true)
    { m_loaded = _loaded; }

    /*!
     * \brief Получение радиуса сферы, в которую вписана модель
     * \return Возвращает радиус сферы
     */
    inline float modelRadius()
    {  return m_modelRadius; }

    /*!
     * \brief Получение мешей модели
     * \return Возвращает меши модели
     */
    inline const QVector<FuryMesh*>& meshes() const
    { return m_meshes; }

    //! Подключение мешей к OpenGL
    void setupMesh();

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
    FuryMesh* processMesh(aiMesh* _mesh, const aiScene* _scene, const aiNode* _node);

    /*!
     * \brief Вычисление габаритов всей модели по минимальным и максимальным
     * координанам
     * \param[in] _vertex - Обрабатываемая вершина
     */
    void calculateMinMaxVertex(const glm::vec3& _vertex);

    /*!
     * \brief Вычисление габаритов меша
     * \param[in] _vertex - Обрабатываемая вершина
     * \param[out] _minVertex - Минимальная координата куба, в которую вписан меш
     * \param[out] _maxVertex - Максимальная координата куба, в которую вписан меш
     */
    void calculateMinMaxForMesh(const glm::vec3& _vertex,
                                glm::vec3& _minVertex,
                                glm::vec3& _maxVertex);

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
