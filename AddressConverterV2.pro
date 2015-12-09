#-------------------------------------------------
#
# Project created by QtCreator 2015-11-17T20:48:33
#
#-------------------------------------------------

QT       += core gui sql axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AddressConverterV2
TEMPLATE = app

#DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp \
    parser.cpp \
    searcher.cpp \
    address.cpp \
    database.cpp \
    databasewidget.cpp \
    csvworker.cpp \
    debugwidget.cpp \
    tabwidget.cpp \
    tableview.cpp \
    excelwidget.cpp \
    xlsworker.cpp \
    xlsparser.cpp \
    tablemodel.cpp

HEADERS  += mainwindow.h \
    parser.h \
    searcher.h \
    defines.h \
    address.h \
    database.h \
    databasewidget.h \
    csvworker.h \
    debugwidget.h \
    tabwidget.h \
    tableview.h \
    excelwidget.h \
    xlsworker.h \
    xlsparser.h \
    tablemodel.h

FORMS    += mainwindow.ui \
    databasewidget.ui \
    debugwidget.ui \
    excelwidget.ui

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
