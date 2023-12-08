TARGET = FuryEngine
TEMPLATE = app
VERSION = 0.0.1

QT += core gui widgets opengl openglwidgets

CONFIG += c++17

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build


INCLUDEPATH += includes
LIBS        += -Llibs \
               -lglew32s \
               -lOpenGL32 \
               -lglu32 \
               -lSOIL \
               -lassimp-vc143-mtd \
               -lreactphysics3d


SOURCES += \
    src/BoxObject.cpp \
    src/CarObject.cpp \
    src/FuryException.cpp \
    src/FuryLogger.cpp \
    src/FuryRaycastCallback.cpp \ #src/FuryWindow.cpp \
    src/Object.cpp \
    src/Particle.cpp \
    src/ParticleSystem.cpp \
    src/SphereObject.cpp \
    src/TestRender.cpp \
    src/Texture.cpp \
    src/TextureManager.cpp \
    src/World.cpp \
    src/main.cpp \
    src/MainWindow.cpp

HEADERS += \
    src/BoxObject.h \
    src/Camera.h \
    src/CarObject.h \
    src/FuryException.h \
    src/FuryLogger.h \
    src/FuryRaycastCallback.h \ #src/FuryWindow.h \
    src/MainWindow.h \
    src/Object.h \
    src/Particle.h \
    src/ParticleSystem.h \
    src/Shader.h \
    src/SphereObject.h \
    src/TestRender.h \
    src/Texture.h \
    src/TextureLoader.h \
    src/TextureManager.h \
    src/World.h \
    src/mesh.h \
    src/model.h

FORMS += \
    src/MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
