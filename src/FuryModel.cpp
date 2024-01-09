#include "FuryModel.h"

#include "FuryLogger.h"
#include "FuryMaterial.h"
#include "FuryMaterialManager.h"

#include <QFileInfo>

FuryModel::FuryModel(const QString &_path) :
    m_path(_path),
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

void FuryModel::draw(Shader *_shader, FuryMaterial *_material)
{
    for (FuryMesh* mesh : m_meshes)
    {
        mesh->draw(_shader, _material);
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
        m_meshes.push_back(processMesh(mesh, _scene));
    }

    // Обработаем меши дочерних мешей
    for (unsigned int i = 0; i < _node->mNumChildren; i++)
    {
        processNode(_node->mChildren[i], _scene);
    }
}

FuryMesh *FuryModel::processMesh(aiMesh *_mesh, const aiScene *_scene)
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
    QString materialName = "Model:" + ru(material->GetName().C_Str());

    if (!materialManager->materialExist(materialName))
    {
        FuryMaterial* modelMaterial = FuryMaterial::createFromAssimp(material, m_directory);
        FuryMaterial* material = materialManager->createMaterial(materialName);
        *material = *modelMaterial;
    }

    FuryMaterial* furyMaterial = materialManager->materialByName(materialName);

    // Возвращаем созданный меш
    return new FuryMesh(vertices, indices, furyMaterial);
}
