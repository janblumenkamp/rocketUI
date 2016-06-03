#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T09:58:59
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RocketUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    comm.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    comm.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui
