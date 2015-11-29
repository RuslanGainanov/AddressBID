#-------------------------------------------------
#
# Project created by QtCreator 2015-11-17T20:48:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AddressConverterV2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    parser.cpp \
    searcher.cpp \
    address.cpp \
    database.cpp \
    databasewidget.cpp \
    csvworker.cpp

HEADERS  += mainwindow.h \
    parser.h \
    searcher.h \
    defines.h \
    address.h \
    database.h \
    databasewidget.h \
    csvworker.h

FORMS    += mainwindow.ui \
    databasewidget.ui

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
