#-------------------------------------------------
#
# Project created by QtCreator 2017-01-02T20:50:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Convert2Excel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fileparser.cpp

HEADERS  += mainwindow.h \
    fileparser.h

FORMS    += mainwindow.ui
