#-------------------------------------------------
#
# Project created by QtCreator 2015-06-02T05:44:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SignalTest
TEMPLATE = app


SOURCES += main.cpp\
    signalwindow.cpp \
    signaldialog.cpp \
    stone.cpp \
    xorstar.cpp \
    mainwindow.cpp

HEADERS  += signalwindow.h \
    signaldialog.h \
    constant.h \
    stone.h \
    xorstar.h \
    mainwindow.h

FORMS    += signalwindow.ui \
    signaldialog.ui \

RESOURCES += \
    stone.qrc
