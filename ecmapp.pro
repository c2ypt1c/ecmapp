#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27T07:05:33
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ecmapp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vetable.cpp \
    maftable.cpp \
    cvsimport.cpp

HEADERS  += mainwindow.h \
    vetable.h \
    maftable.h \
    cvsimport.h

FORMS    += mainwindow.ui
