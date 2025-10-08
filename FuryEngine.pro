TARGET = FuryEngine
TEMPLATE = app
VERSION = 0.0.1

QT += core gui widgets opengl openglwidgets

CONFIG += c++17 console

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build


INCLUDEPATH += \
    src \
    includes \
    libs/python/include


# Выкрутасы из-за одинаковых названий библиотеки в debug и release
CONFIG(debug, debug|release) {
    LIBS        += \
        -Llibs/reactphysics3d/debug \
}else {
    LIBS        += \
        -Llibs/reactphysics3d/release \
}


LIBS        += \
    -Llibs \
    -Llibs/assimp \
    -Llibs/boost \
    -Llibs/glew \
    -Llibs/python \
    -Llibs/python/libs \
    -lglew32 \
    -lOpenGL32 \
    -lassimp \
    -lreactphysics3d


SOURCES += \
    src/Camera.cpp \
    src/CarObject.cpp \
    src/FuryLearningScript.cpp \
    src/FuryObjectsFactory.cpp \
    src/FuryPhongMaterial.cpp \
    src/LocalKeyboard/FuryBaseLocalKeyMapper.cpp \
    src/DefaultObjects/FuryBoxObject.cpp \
    src/Managers/FuryScriptManager.cpp \
    src/Managers/FuryShaderManager.cpp \
    src/Managers/FuryWorldManager.cpp \
    src/PythonAPI/CarObject_Python.cpp \
    src/PythonAPI/FuryObject_Python.cpp \
    src/PythonAPI/GLM_Python.cpp \
    src/Shader.cpp \
    src/Widgets/FuryDoubleValidator.cpp \
    src/Physics/FuryEventListener.cpp \
    src/Logger/FuryException.cpp \
    src/Logger/FuryLogger.cpp \
    src/Widgets/FuryMainWindow.cpp \
    src/FuryMaterial.cpp \
    src/Widgets/FuryManagersDataDialog.cpp \
    src/Widgets/FuryMaterialEditDialog.cpp \
    src/Managers/FuryMaterialManager.cpp \
    src/FuryMesh.cpp \
    src/FuryModel.cpp \
    src/Managers/FuryModelManager.cpp \
    src/FuryObject.cpp \
    src/Widgets/FuryMaterialRenderWidget.cpp \
    src/Widgets/FuryObjectsTreeModel.cpp \
    src/FuryPbrMaterial.cpp \
    src/Physics/FuryRaycastCallback.cpp \
    src/LocalKeyboard/FuryRussianLocalKeyMapper.cpp \
    src/DefaultObjects/FurySphereObject.cpp \
    src/FuryTexture.cpp \
    src/Managers/FuryTextureManager.cpp \
    src/Widgets/FuryRenderer.cpp \
    src/Widgets/FuryVectorInputWidget.cpp \
    src/FuryWorld.cpp \
    src/Particle.cpp \
    src/ParticleSystem.cpp \
    src/main.cpp

HEADERS += \
    src/Camera.h \
    src/CarObject.h \
    src/FuryLearningScript.h \
    src/FuryObjectsFactory.h \
    src/FuryPhongMaterial.h \
    src/LocalKeyboard/FuryBaseLocalKeyMapper.h \
    src/DefaultObjects/FuryBoxObject.h \
    src/Managers/FuryScriptManager.h \
    src/Managers/FuryShaderManager.h \
    src/Managers/FuryWorldManager.h \
    src/Widgets/FuryDoubleValidator.h \
    src/Physics/FuryEventListener.h \
    src/Logger/FuryException.h \
    src/Logger/FuryLogger.h \
    src/Widgets/FuryMainWindow.h \
    src/FuryMaterial.h \
    src/Widgets/FuryManagersDataDialog.h \
    src/Widgets/FuryMaterialEditDialog.h \
    src/Managers/FuryMaterialManager.h \
    src/FuryMesh.h \
    src/FuryModel.h \
    src/Managers/FuryModelManager.h \
    src/FuryObject.h \
    src/Widgets/FuryMaterialRenderWidget.h \
    src/Widgets/FuryObjectsTreeModel.h \
    src/FuryPbrMaterial.h \
    src/Physics/FuryRaycastCallback.h \
    src/LocalKeyboard/FuryRussianLocalKeyMapper.h \
    src/DefaultObjects/FurySphereObject.h \
    src/FuryTexture.h \
    src/Managers/FuryTextureManager.h \
    src/Widgets/FuryRenderer.h \
    src/Widgets/FuryVectorInputWidget.h \
    src/FuryWorld.h \
    src/Particle.h \
    src/ParticleSystem.h \
    src/Shader.h

FORMS += \
    src/Widgets/FuryMainWindow.ui \
    src/Widgets/FuryManagersDataDialog.ui \
    src/Widgets/FuryMaterialEditDialog.ui \
    src/Widgets/FuryVectorInputWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    FuryRecources.qrc
