#-------------------------------------------------
#
# Project created by QtCreator 2016-07-26T04:51:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 2048
TEMPLATE = app


SOURCES += main.cpp\
        gamewidget/gamewidget.cpp \
    kernel/game.cpp \
    kernel/tablet.cpp \
    gamewidget/widgetstate.cpp \
    application.cpp \
    mainwindow.cpp \
    settingsboard.cpp \
    scoreboard.cpp \
    gameboard.cpp \
    scorelabel.cpp \
    kernel/gameobject.cpp

HEADERS  += gamewidget/gamewidget.h \
    kernel/game.h \
    kernel/tablet.h \
    gamewidget/widgetstate.h \
    application.h \
    mainwindow.h \
    settingsboard.h \
    scoreboard.h \
    gameboard.h \
    scorelabel.h \
    kernel/gameobject.h
