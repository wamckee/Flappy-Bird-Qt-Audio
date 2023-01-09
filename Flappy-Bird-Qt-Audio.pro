# Flappy Bird Qt with Audio control
# github.com/wamckee/Flappy-Bird-Qt-Audio

QT += core gui opengl multimedia
QT -= network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

TARGET = Flappy-Bird-Qt-Audio
TEMPLATE = app

VERSION = 1.0.0

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/source

INCLUDEPATH += \ # submodules
               $$PWD/source/tiny-dnn-1 \
               $$PWD/source/strobe-api-1/strobe

win32:RC_ICONS += assets/icon.ico

CONFIG += c++14

SOURCES += \
    source/MainWindow/MainWindow.cpp \
    source/Bird/Bird.cpp \
    source/Button/Button.cpp \
    source/Game/Game.cpp \
    source/Physics/Physics.cpp \
    source/Scene/Scene.cpp \
    source/main.cpp \
    source/Sound/Sound.cpp \
    source/View/View.cpp \
    source/StrobeDialog/StrobeDialog.cpp \
    source/AI/AI.cpp

HEADERS += \
    source/MainWindow/MainWindow.h \
    source/Bird/Bird.h \
    source/Button/Button.h \
    source/Game/Game.h \
    source/Physics/Physics.h \
    source/Scene/Scene.h \
    source/common.h \
    source/Sound/Sound.h \
    source/View/View.h \
    source/StrobeDialog/StrobeDialog.h \
    source/AI/AI.h \
    source/vector.h

RESOURCES += \
    assets/Resource.qrc

