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
        gamewidget.cpp \
    kernel/game.cpp \
    state.cpp

HEADERS  += gamewidget.h \
    kernel/game.h \
    state.h
