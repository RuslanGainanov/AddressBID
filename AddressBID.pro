QT       += core gui sql axcontainer concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AddressBID
TEMPLATE = app

#DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp \
    searcher.cpp \
    address.cpp \
    database.cpp \
    databasewidget.cpp \
    debugwidget.cpp \
    excelwidget.cpp \
    xlsparser.cpp \
    tablemodel.cpp \
    parseexcelwidget.cpp \
    helpbrowser.cpp \
    simpledelegate.cpp

HEADERS  += mainwindow.h \
    searcher.h \
    defines.h \
    address.h \
    database.h \
    databasewidget.h \
    debugwidget.h \
    excelwidget.h \
    xlsparser.h \
    tablemodel.h \
    parseexcelwidget.h \
    helpbrowser.h \
    simpledelegate.h

FORMS    += mainwindow.ui \
    databasewidget.ui \
    debugwidget.ui \
    excelwidget.ui \
    helpbrowser.ui \
    parseexcelwidget.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += file.rc
win32:RC_FILE += file.rc

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

DISTFILES += \
    doc/img/image001.png \
    doc/img/image002.png \
    doc/img/image003.png \
    doc/img/image004.png \
    doc/img/image007.png \
    doc/img/image008.png \
    doc/img/image009.png \
    doc/img/image010.png \
    README.txt \
    README.md \
    deploy/AddressBID_setup_v2.iss \
    deploy/deploy.cmd
