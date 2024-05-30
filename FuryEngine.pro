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
    includes \
    $$(LOCALAPPDATA)/Programs/Python/Python36/include


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
    -L$$(LOCALAPPDATA)/Programs/Python/Python36 \
    -L$$(LOCALAPPDATA)/Programs/Python/Python36/libs \
    -lglew32 \
    -lOpenGL32 \
    -lassimp \
    -lreactphysics3d


SOURCES += \
    src/Camera.cpp \
    src/CarObject.cpp \
    src/FuryBaseLocalKeyMapper.cpp \
    src/FuryBoxObject.cpp \
    src/FuryEventListener.cpp \
    src/FuryException.cpp \
    src/FuryLogger.cpp \
    src/FuryMainWindow.cpp \
    src/FuryMaterial.cpp \
    src/FuryMaterialEditDialog.cpp \
    src/FuryMaterialManager.cpp \
    src/FuryMesh.cpp \
    src/FuryModel.cpp \
    src/FuryModelManager.cpp \
    src/FuryObject.cpp \
    src/FuryPbrMaterial.cpp \
    src/FuryRaycastCallback.cpp \ #src/FuryWindow.cpp \
    src/FuryRussianLocalKeyMapper.cpp \
    src/FuryScript.cpp \
    src/FurySphereObject.cpp \
    src/FuryTexture.cpp \
    src/FuryTextureManager.cpp \
    src/FuryWorld.cpp \
    src/Particle.cpp \
    src/ParticleSystem.cpp \
    src/TestRender.cpp \
    src/main.cpp

HEADERS += \
    src/Camera.h \
    src/CarObject.h \
    src/FuryBaseLocalKeyMapper.h \
    src/FuryBoxObject.h \
    src/FuryEventListener.h \
    src/FuryException.h \
    src/FuryLogger.h \
    src/FuryMainWindow.h \
    src/FuryMaterial.h \
    src/FuryMaterialEditDialog.h \
    src/FuryMaterialManager.h \
    src/FuryMesh.h \
    src/FuryModel.h \
    src/FuryModelManager.h \
    src/FuryObject.h \
    src/FuryPbrMaterial.h \
    src/FuryRaycastCallback.h \ #src/FuryWindow.h \
    src/FuryRussianLocalKeyMapper.h \
    src/FuryScript.h \
    src/FurySphereObject.h \
    src/FuryTexture.h \
    src/FuryTextureManager.h \
    src/FuryWorld.h \
    src/Particle.h \
    src/ParticleSystem.h \
    src/Shader.h \
    src/TestRender.h

FORMS += \
    src/FuryMainWindow.ui \
    src/FuryMaterialEditDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    FuryRecources.qrc
