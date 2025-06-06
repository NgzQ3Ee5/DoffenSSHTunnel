#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T12:04:37
#
#-------------------------------------------------
cache()

QT       += core gui widgets

macx:QT += macextras

win32-msvc2008:QMAKE_LFLAGS += /INCREMENTAL
win32-msvc2010:QMAKE_LFLAGS += /INCREMENTAL
win32-msvc2012:QMAKE_LFLAGS += /INCREMENTAL
win32-msvc2013:QMAKE_LFLAGS += /INCREMENTAL
win64-msvc2022:QMAKE_LFLAGS += /INCREMENTAL

TARGET = DoffenSSHTunnel
TEMPLATE = app
RC_ICONS = icon_128x128.ico

include(botan/botan.pri)
include(pwdlemont/pwdlemont.pri)
include(source.pri)


