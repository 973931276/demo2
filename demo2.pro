#-------------------------------------------------
#
# Project created by QtCreator 2022-02-13T21:29:08
#
#-------------------------------------------------

QT       += core gui multimedia
QT       += multimedia
QT       += network
QT       += charts
QT       += 3dinput
QT       += printsupport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    download.cpp \
    wavefile.cpp \
    xyseriesiodevice.cpp \
    mic.cpp \
    praat.cpp

HEADERS += \
        mainwindow.h \
    download.h \
    wavefile.h \
    mygraph.h \
    xyseriesiodevice.h \
    mic.h \
    praat.h

FORMS += \
        mainwindow.ui \
    download.ui \
    mic.ui \
    praat.ui

RESOURCES += \
    res.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/charts/callout
INSTALLS += target

DISTFILES += \
    wav.py
