#ifndef FURYMODELMANAGER_H
#define FURYMODELMANAGER_H

#include "FuryModel.h"

#include <QString>
#include <QMap>
#include <QQueue>


class FuryModelManager
{
public:
    static FuryModelManager* instance();
    static FuryModelManager* createInstance();
    static void deleteInstance();

    void addModel(const QString& _path, const QString& _name = "");
    FuryModel* modelByName(const QString& _name);
    FuryModel* modelByPath(const QString& _path);

    void loadModelPart();

    void stopLoopAndWait();

private:
    FuryModelManager();
    ~FuryModelManager();
    static FuryModelManager* s_instance;

    void infiniteLoop();

    bool m_needStop;
    bool m_stopped;

private:
    QMap<QString, FuryModel*> m_models;
    QMap<QString, QString> m_nameToPath;

    QQueue<FuryModel*> m_modelLoadQueue;
    QQueue<FuryModel*> m_modelBindQueue;

    FuryModel m_emptyModel;
};

#endif // FURYMODELMANAGER_H
