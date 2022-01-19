#-------------------------------------------------
#
# Project created by QtCreator 2021-10-29T13:38:06
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 2021-JCO-CasseBrique
TEMPLATE = app
RC_ICONS = icon.ico

#DEFINES += DEBUG_SPRITE_COUNT
#DEFINES += DEBUG_BBOX
#DEFINES += DEBUG_SHAPE
#DEFINES += DEPLOY # Pour une compilation dans un but de déploiement

SOURCES += main.cpp\
    ball.cpp \
        mainfrm.cpp \
    gamescene.cpp \
    plate.cpp \
    sprite.cpp \
    gamecore.cpp \
    resources.cpp \
    gameview.cpp \
    utilities.cpp \
    gamecanvas.cpp \
    spritetickhandler.cpp \
    bouncingspritehandler.cpp

HEADERS  += mainfrm.h \
    ball.h \
    gamescene.h \
    plate.h \
    sprite.h \
    gamecore.h \
    resources.h \
    gameview.h \
    utilities.h \
    gamecanvas.h \
    spritetickhandler.h \
    bouncingspritehandler.h

FORMS    += mainfrm.ui
