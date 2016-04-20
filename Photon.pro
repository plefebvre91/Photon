#-------------------------------------------------
#
# Project created by QtCreator 2016-04-16T13:49:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Photon
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
    qcustomplot.cpp \
    photon.cpp \
    photonplot.cpp

HEADERS  += \
    qcustomplot.h \
    photon.h \
    photonplot.h \
    defs.h


LIBS += -lexif

FORMS    += mainwindow.ui
