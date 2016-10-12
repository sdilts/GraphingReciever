#-------------------------------------------------
#
# Project created by QtCreator 2016-10-06T13:15:18
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

TARGET = GraphingReciever
TEMPLATE = app


SOURCES += main.cpp\
        datalogger.cpp \
    qcustomplot.cpp \
    reciever.cpp

HEADERS  += datalogger.h \
    qcustomplot.h \
    reciever.h \
    constants.h

FORMS    += datalogger.ui
