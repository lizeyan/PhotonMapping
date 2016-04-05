TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += main.cpp \
    condutor.cpp \
    light.cpp \
    object.cpp \
    camera.cpp \
    photonmap.cpp \
    image.cpp \
    material.cpp

HEADERS += \
    condutor.h \
    light.h \
    object.h \
    camera.h \
    photonmap.h \
    color.h \
    image.h \
    vector.h \
    ray.h \
    material.h \
    base.h
