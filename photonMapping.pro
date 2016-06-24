TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= QT
CONFIG += DEBUG

SOURCES += main.cpp \
    condutor.cpp \
    light.cpp \
    object.cpp \
    camera.cpp \
    image.cpp \
    material.cpp \
    primitive.cpp \
    tracer.cpp \
    raytracer.cpp \
    ray.cpp \
    vector.cpp \
    objectparser.cpp \
    photontracer.cpp \
    photonmap.cpp \
    triangleBox.cpp \
    triangleTree.cpp \
    photonbox.cpp

HEADERS += \
    condutor.h \
    light.h \
    object.h \
    camera.h \
    color.h \
    image.h \
    vector.h \
    ray.h \
    material.h \
    base.h \
    primitive.h \
    tracer.h \
    raytracer.h \
    objectparser.h \
    photontracer.h \
    photonmap.h \
    triangleBox.h \
    triangleTree.h \
    photonbox.h

DISTFILES +=
