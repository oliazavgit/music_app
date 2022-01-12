#-------------------------------------------------
#
# Project created by QtCreator 2019-10-11T14:02:15
#
#-------------------------------------------------

QT       += core gui sql multimedia network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pkserver
TEMPLATE = app

# Настройка места сборки--------------------------
BUILD_DIR   = $${PWD}/build
OBJECTS_DIR = $${BUILD_DIR}/obj
MOC_DIR     = $${BUILD_DIR}/moc
RCC_DIR     = $${BUILD_DIR}/rcc
UI_DIR      = $${BUILD_DIR}/ui
DESTDIR     = exe

SOURCES += main.cpp\
    dgrafic.cpp \
        mwmain.cpp \
    dreg.cpp \
    dexec.cpp \
    dtrack.cpp \
    dstyle.cpp \
    dabout.cpp

HEADERS  += mwmain.h \
    dgrafic.h \
    dreg.h \
    dexec.h \
    dtrack.h \
    dstyle.h \
    dabout.h

FORMS    += mwmain.ui \
    dgrafic.ui \
    dreg.ui \
    dexec.ui \
    dtrack.ui \
    dstyle.ui \
    dabout.ui

RESOURCES += \
    res.qrc
