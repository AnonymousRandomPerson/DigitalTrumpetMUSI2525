#-------------------------------------------------
#
# Project created by QtCreator 2016-10-11T11:36:52
#
#-------------------------------------------------

QT       += core gui\
            multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DigitalTrumpetGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        ../../C++/TrumpetGenerator.cpp \
        audioplayer.cpp
        #../../C++/Dummy.cpp

HEADERS  += mainwindow.h\
         ../../C++/TrumpetGenerator.h \
         audioplayer.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../C++
