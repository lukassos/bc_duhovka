#-------------------------------------------------
#
# Project created by QtCreator 2012-01-17T21:28:01
#
#-------------------------------------------------

QT       += core gui

TARGET = duhovecka
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagemanip.cpp \
    snake.cpp \
    findFiles.cpp

HEADERS  += mainwindow.h \
    imagemanip.h \
    snake.h \
    findFiles.h

FORMS    += mainwindow.ui

win32: INCLUDEPATH   +=  C:\opencv22mingw\include
win32: LIBS += -LC:\opencv22mingw\bin
LIBS += -lopencv_core220 \
        -lopencv_highgui220 \
        -lopencv_imgproc220







