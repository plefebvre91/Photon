#-------------------------------------------------
#
# Project created by QtCreator 2016-04-16T13:49:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Photon
TEMPLATE = app


SOURCES += main.cpp\
    qcustomplot.cpp \
    photon.cpp

HEADERS  += \
    qcustomplot.h \
    photon.h

LIBS += -lexif

FORMS    += mainwindow.ui
