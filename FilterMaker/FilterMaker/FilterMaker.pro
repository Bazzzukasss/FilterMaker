#-------------------------------------------------
#
# Project created by QtCreator 2016-01-07T18:03:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = FilterMaker
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    filterdevice.cpp \
    filter.cpp \
    dataloader.cpp

HEADERS  += mainwindow.h \
    filter.h \
    qcustomplot.h \
    filterdevice.h \
    dataloader.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -std=c++14

RESOURCES += \
    images.qrc
