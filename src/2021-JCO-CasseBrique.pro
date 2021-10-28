#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T13:38:06
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameFramework
TEMPLATE = app

#DEFINES += DEBUG_SPRITE_COUNT
#DEFINES += DEBUG_BBOX
#DEFINES += DEBUG_SHAPE
#DEFINES += DEPLOY # Pour une compilation dans un but de déploiement

SOURCES += main.cpp\
    blueball.cpp \
    keytickhandler.cpp \
        mainfrm.cpp \
    gamescene.cpp \
    plate.cpp \
    player.cpp \
    randommovetickhandler.cpp \
    sprite.cpp \
    gamecore.cpp \
    resources.cpp \
    gameview.cpp \
    utilities.cpp \
    gamecanvas.cpp \
    walkingman.cpp \
    spinningpinwheel.cpp \
    spritetickhandler.cpp \
    bouncingspritehandler.cpp \
    manualwalkinghandler.cpp \
    automaticwalkinghandler.cpp

HEADERS  += mainfrm.h \
    blueball.h \
    gamescene.h \
    keytickhandler.h \
    plate.h \
    player.h \
    randommovetickhandler.h \
    sprite.h \
    gamecore.h \
    resources.h \
    gameview.h \
    utilities.h \
    gamecanvas.h \
    walkingman.h \
    spinningpinwheel.h \
    spritetickhandler.h \
    bouncingspritehandler.h \
    manualwalkinghandler.h \
    automaticwalkinghandler.h

FORMS    += mainfrm.ui
