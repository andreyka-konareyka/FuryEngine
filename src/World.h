#ifndef WORLD_H
#define WORLD_H


#include <vector>
#include <string>
#include <map>

#include <QVector>

#include "Camera.h"
#include "Object.h"
#include "Texture.h"

class World {
public:
    World();
    ~World();
    void Tick(double _dt);
    void addObject(Object* _object);
    void setCamera(Camera* _camera);
    void deleteCurrentCamera();

    const QVector<Object*>& getObjects();
    inline Camera* camera() const
    { return m_currentCamera; }


private:
    Camera* m_currentCamera;
    QVector<Object*> m_objects;
};

#endif // WORLD_H
