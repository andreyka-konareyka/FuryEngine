#ifndef FURYMATERIALMANAGER_H
#define FURYMATERIALMANAGER_H

#include <QMap>
#include <QString>

class FuryMaterial;


class FuryMaterialManager
{
public:
    static FuryMaterialManager* instance();
    static FuryMaterialManager* createInstance();
    static void deleteInstance();

    FuryMaterial* createMaterial(const QString& _name);
    FuryMaterial* materialByName(const QString& _name);

    bool materialExist(const QString& _name);

private:
    FuryMaterialManager();
    ~FuryMaterialManager();
    static FuryMaterialManager* s_instance;

private:
    QMap<QString, FuryMaterial*> m_materials;
    FuryMaterial* m_defaultMaterial;
};

#endif // FURYMATERIALMANAGER_H
