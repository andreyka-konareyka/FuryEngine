#include "FuryModel.h"

#include "Shader.h"
#include "FuryLogger.h"
#include "FuryMaterial.h"
#include "FuryPbrMaterial.h"
#include "FuryMaterialManager.h"

#include <QFileInfo>
#include <numeric>

FuryModel::FuryModel(const QString &_path) :
    m_path(_path),
    m_minimumVertex(std::numeric_limits<float>::max()),
    m_maximumVertex(std::numeric_limits<float>::min()),
    m_ready(false),
    m_loaded(false)
{

}

FuryModel::FuryModel() :
    m_ready(false),
    m_loaded(false)
{

}

FuryModel::~FuryModel()
{
    for (FuryMesh* mesh : m_meshes)
    {
        delete mesh;
    }
}

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{

    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (abs(LocalMatrix[3][3]) <= 0.001)
        return false;

    // First, isolate perspective.  This is the messiest.
    if (
        abs(LocalMatrix[0][3]) >= 0.001 ||
        abs(LocalMatrix[1][3]) >= 0.001 ||
        abs(LocalMatrix[2][3]) >= 0.001)
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3];

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    glm::normalize(Row[0]); // , static_cast<T>(1));
    scale.y = length(Row[1]);
    glm::normalize(Row[1]); // , static_cast<T>(1));
    scale.z = length(Row[2]);
    glm::normalize(Row[2]); // , static_cast<T>(1));

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }


    return true;
}


void FuryModel::draw(Shader *_shader, const glm::mat4 &_transformation,
                     FuryMaterial *_material)
{
    for (FuryMesh* mesh : m_meshes)
    {
        glm::mat4 modelMatrix = _transformation * mesh->transformation();

        if (m_path.endsWith("fbx"))
        {
            glm::mat4 modelTransform = glm::translate(glm::mat4(1), glm::vec3(0, 10, 0));
            modelTransform = glm::scale(modelTransform, glm::vec3(0.01, 0.01, 0.01));
            modelMatrix = _transformation * modelTransform * mesh->transformation();
        }

        _shader->setMat4("model", modelMatrix);
        _shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));
        mesh->draw(_shader, _material);

    }
}

void FuryModel::drawShadowMap()
{
    for (FuryMesh* mesh : m_meshes)
    {
        mesh->drawShadowMap();
    }
}

void FuryModel::setupMesh()
{
    for (FuryMesh* mesh : m_meshes)
    {
        mesh->setupMesh();
    }

    m_ready = true;
}

bool FuryModel::loadModel()
{
    // Читаем файл модели
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(qUtf8Printable(m_path),
                                             aiProcess_Triangulate
                                             | aiProcess_GenSmoothNormals
                                             | aiProcess_FlipUVs
                                             | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Debug("ERROR::ASSIMP:: ") << importer.GetErrorString();
        return false;
    }

    QFileInfo fileInfo(m_path);
    m_directory = fileInfo.absolutePath() + "/";

    processNode(scene->mRootNode, scene);
    calculateRadius();

    m_loaded = true;
    return true;
}

void FuryModel::processNode(aiNode *_node, const aiScene *_scene)
{
    // Обработка наши меши
    for (unsigned int i = 0; i < _node->mNumMeshes; i++)
    {
        // Получение меша по индексу
        aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, _scene, _node));
    }

    // Обработаем меши дочерних мешей
    for (unsigned int i = 0; i < _node->mNumChildren; i++)
    {
        processNode(_node->mChildren[i], _scene);
    }
}

glm::mat4 AiToGLMMat4(const aiMatrix4x4& in_mat)
{
    glm::mat4 tmp;
    tmp[0][0] = in_mat.a1;
    tmp[0][1] = in_mat.b1;
    tmp[0][2] = in_mat.c1;
    tmp[0][3] = in_mat.d1;

    tmp[1][0] = in_mat.a2;
    tmp[1][1] = in_mat.b2;
    tmp[1][2] = in_mat.c2;
    tmp[1][3] = in_mat.d2;

    tmp[2][0] = in_mat.a3;
    tmp[2][1] = in_mat.b3;
    tmp[2][2] = in_mat.c3;
    tmp[2][3] = in_mat.d3;

    tmp[3][0] = in_mat.a4;
    tmp[3][1] = in_mat.b4;
    tmp[3][2] = in_mat.c4;
    tmp[3][3] = in_mat.d4;
    return tmp;
}

FuryMesh *FuryModel::processMesh(aiMesh *_mesh, const aiScene *_scene, const aiNode *_node)
{
    // Заполнение данных
    QVector<FuryMesh::Vertex> vertices;
    QVector<unsigned int> indices;

    // Обходим вершины
    for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
    {
        FuryMesh::Vertex vertex;
        // Позиция
        vertex.m_position = glm::vec3(_mesh->mVertices[i].x,
                                      _mesh->mVertices[i].y,
                                      _mesh->mVertices[i].z);
        calculateMinMaxVertex(vertex.m_position);

        // Нормаль
        if (_mesh->HasNormals())
        {
            vertex.m_normal = glm::vec3(_mesh->mNormals[i].x,
                                        _mesh->mNormals[i].y,
                                        _mesh->mNormals[i].z);
        }

        // Текстурные координаты
        // Предполагаем, что используем только первый набор текстурных координат
        if (_mesh->HasTextureCoords(0))
        {
            vertex.m_texCoords = glm::vec2(_mesh->mTextureCoords[0][i].x,
                                           _mesh->mTextureCoords[0][i].y);
            // Тангент
            vertex.m_tangent = glm::vec3(_mesh->mTangents[i].x,
                                         _mesh->mTangents[i].y,
                                         _mesh->mTangents[i].z);
            // Битангент
            vertex.m_bitangent = glm::vec3(_mesh->mBitangents[i].x,
                                           _mesh->mBitangents[i].y,
                                           _mesh->mBitangents[i].z);
        }
        else
        {
            vertex.m_texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Обходим поверхности и строим индексы
    for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
    {
        aiFace face = _mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Обработка материала
    FuryMaterialManager* materialManager = FuryMaterialManager::instance();
    aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];
    QString materialName = "Model." + ru(material->GetName().C_Str());

    if (!materialManager->materialExist(materialName))
    {
        if (!materialManager->tryLoadMaterial(materialName))
        {
            FuryMaterial* modelMaterial = FuryMaterial::createFromAssimp(material, m_directory);
//            FuryMaterial* material = materialManager->createMaterial(materialName);
//            *material = *modelMaterial;

            FuryPbrMaterial* pbrMat = FuryPbrMaterial::createFromMaterial(modelMaterial);
            FuryPbrMaterial* mat = materialManager->createPbrMaterial(materialName);
            *mat = *pbrMat;
            delete pbrMat;
            delete modelMaterial;
        }
    }

    glm::mat4 transform = AiToGLMMat4(_node->mTransformation);

    {
        aiNode* parent = _node->mParent;

        while (parent != _scene->mRootNode)
        {
            transform = AiToGLMMat4(parent->mTransformation) * transform;
            parent = parent->mParent;
        }
    }

    // Возвращаем созданный меш
    return new FuryMesh(vertices, indices, materialName, transform);
}

void FuryModel::calculateMinMaxVertex(const glm::vec3 &_vertex)
{
    // Смотрим минимальную координату
    if (_vertex.x < m_minimumVertex.x)
    {
        m_minimumVertex.x = _vertex.x;
    }

    if (_vertex.y < m_minimumVertex.y)
    {
        m_minimumVertex.y = _vertex.y;
    }

    if (_vertex.z < m_minimumVertex.z)
    {
        m_minimumVertex.z = _vertex.z;
    }

    // Смотрим максимальную координату
    if (_vertex.x > m_maximumVertex.x)
    {
        m_maximumVertex.x = _vertex.x;
    }

    if (_vertex.y > m_maximumVertex.y)
    {
        m_maximumVertex.y = _vertex.y;
    }

    if (_vertex.z > m_maximumVertex.z)
    {
        m_maximumVertex.z = _vertex.z;
    }
}

void FuryModel::calculateRadius()
{
    float radiusMin = glm::length(m_minimumVertex);
    float radiusMax = glm::length(m_maximumVertex);

    m_modelRadius = std::max(radiusMin, radiusMax);
}
