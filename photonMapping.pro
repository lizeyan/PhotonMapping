TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= QT

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
    kdtree.cpp \
    kdnode.cpp \
    objectparser.cpp

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
    kdtree.h \
    kdnode.h \
    objectparser.h
