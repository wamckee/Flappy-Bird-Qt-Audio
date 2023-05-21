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
               $$PWD/source/tiny-dnn \
               $$PWD/source/strobe-api/strobe

INCLUDEPATH += bounce rtaudio Yin-Pitch-Tracking fir

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
    source/AI/AI.cpp \
    bounce/bounceAudio.cpp \
    rtaudio/RtAudio.cpp \
    Yin-Pitch-Tracking/Yin.c \
    fir/cheby.cpp fir/filter.cpp

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
    source/vector.h \
    bounceAudio.h

RESOURCES += \
    assets/Resource.qrc

mac {
  QMAKE_INFO_PLIST = flappy.Info.plist
  DEFINES += __MACOSX_CORE__
  LIBS += -framework CoreAudio
  LIBS += -framework Foundation
}
else {
  unix {
  DEFINES += _LINUX_PULSE__
  LIBS += -lpulse -lpulse-simple
  }
}
win32 {
  DEFINES += __WINDOWS_DS__
  LIBS += User32.lib Ole32.lib Dsound.lib
}

