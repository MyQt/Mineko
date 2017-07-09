#-------------------------------------------------
#
# Project created by QtCreator 2017-07-02T19:00:57
#
#-------------------------------------------------
VERSION = 1.0.0
QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mineko
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

CONFIG += c++11

SOURCES += main.cpp\
        widget.cpp \
    aristpicture.cpp \
    lyrics.cpp \
    channelswidget.cpp \
    volpannel.cpp \
    getbaiduchannel.cpp \
    getsongplaylistid.cpp \
    getsongreallink.cpp \
    getaristpic.cpp \
    getlrc.cpp \
    getcirclescaledpixmap.cpp \
    eventlabel.cpp \
    downloadfile.cpp \
    playlist.cpp \
    songlistitem.cpp \
    loading.cpp \
    settingsdlg.cpp \
    customtabstyle.cpp \
    skinpicker.cpp \
    lyricpanel.cpp \
    songslider.cpp \
    Updater.cpp


HEADERS  += widget.h \
    aristpicture.h \
    lyrics.h \
    channelswidget.h \
    volpannel.h \
    getbaiduchannel.h \
    getsongplaylistid.h \
    getsongreallink.h \
    getaristpic.h \
    getlrc.h \
    getcirclescaledpixmap.h \
    eventlabel.h \
    downloadfile.h \
    playlist.h \
    songlistitem.h \
    loading.h \
    settingsdlg.h \
    customtabstyle.h \
    skinpicker.h \
    lyricpanel.h \
    songslider.h \
    Updater.h


FORMS    += widget.ui \
    aristpicture.ui \
    lyrics.ui \
    channelswidget.ui \
    playlist.ui \
    loading.ui \
    settingsdlg.ui

RESOURCES += \
    img.qrc

