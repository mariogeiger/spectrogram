#-------------------------------------------------
#
# Project created by QtCreator 2013-07-11T13:18:33
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = spectrogram
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    spectrogram.cpp \
    spectrumanalyser.cpp \
    palette.cpp

HEADERS  += mainwindow.h \
    spectrogram.h \
    spectrumanalyser.h \
    palette.h

LIBS += -lfftw3
