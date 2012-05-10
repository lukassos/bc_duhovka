#-------------------------------------------------
#
# Project created by Lukas Puchon
#                    48016
# 		as part of bachelor thesis
#			Iris segmantation in picture and it`s preprocessing for classifying
#
# Projekt vytvoril Lukas Puchon
#                  48016
# 		ako cast bakalarskej prace
#			Lokalizácia dúhovky v obraze a jej predspracovanie pre úcely klasifikácie	
#
#-------------------------------------------------



QT       += core gui

TARGET = irissnake
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagemanip.cpp \
    snake.cpp \
    findFiles.cpp \
    energyinternaltemplate.cpp \
    energyexternalfield.cpp \
    snakepoint.cpp

HEADERS  += mainwindow.h \
    imagemanip.h \
    snake.h \
    findFiles.h \
    energyinternaltemplate.h \
    energyexternalfield.h \
    crossInclude.h \
    snakepoint.h

FORMS    += mainwindow.ui

win32: INCLUDEPATH   +=  C:\opencv22mingw\include
win32: LIBS += -LC:\opencv22mingw\bin
LIBS += -lopencv_core220 \
        -lopencv_highgui220 \
        -lopencv_imgproc220


















