#-------------------------------------------------
#
# Project created by QtCreator 2019-09-14T22:27:50
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fingerprint
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    setupseriadialog.cpp \
    serialthread.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    setupseriadialog.h \
    serialthread.h \
    fingerprint.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    setupseriadialog.ui
