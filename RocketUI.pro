#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T09:58:59
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RocketUI
TEMPLATE = app

QMAKE_CFLAGS += -std=c99

RESSOURCES += qdarkstyle/style.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    EMBcomm/Unittests.c \
    EMBcomm/PackageMemory.c \
    EMBcomm/Memory.c \
    EMBcomm/HAL_Serial.c \
    EMBcomm/HAL_Memory.c \
    EMBcomm/Comm_Package.c \
    EMBcomm/Comm.c \
    serialinterface.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    EMBcomm/Comm_Package.h \
    EMBcomm/Unittests.h \
    EMBcomm/PackageMemory.h \
    EMBcomm/Memory.h \
    EMBcomm/HAL_Serial.h \
    EMBcomm/HAL_Memory.h \
    EMBcomm/Comm.h \
    serialinterface.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    style.qrc

OTHER_FILES += \
    style.qss
