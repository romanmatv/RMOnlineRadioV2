#-------------------------------------------------
#
# Project created by QtCreator 2018-08-11T12:19:10
#
#-------------------------------------------------

QT       += core gui opengl widgets network printsupport

TARGET = RMOnlineRadioV2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#Windows icon
win32 {
    RC_FILE += file.rc
    OTHER_FILES += file.rc

    VERSION = 1.0.0
}


# Подключаем заголовочные файлы библиотеки
CONFIG(debug, debug|release) {
    # Подключаем debug-версии библиотек для разных платформ
    win32: LIBS += -Llib -lAudioThreadd1
    unix: LIBS += -Llib -L. -lAudioThreadd -Wl,-rpath,lib -Wl,-rpath,.
} else {
    # Подключаем release-версии библиотек для разных платформ
    win32: LIBS += -Llib -lAudioThread1
    unix: LIBS += -Llib -L. -lAudioThread -Wl,-rpath,lib -Wl,-rpath,.
}


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        settingsdialog.cpp \
        gl_paint.cpp \
        sf_paint.cpp \
    inputdialog.cpp \
    crypto.cpp \
    trackmodel.cpp \
    trackdelegate.cpp \
    trackwidget.cpp \
    track.cpp \
    pixmaploader.cpp \
    painter.cpp \
    equalizerwindow.cpp \
    qcustomplot.cpp \
    silentrun.cpp \
    dialog.cpp

HEADERS += \
        mainwindow.h \
        settingsdialog.h \
        gl_paint.h \
        sf_paint.h \
    inputdialog.h \
    crypto.h \
    trackmodel.h \
    trackdelegate.h \
    trackwidget.h \
    track.h \
    pixmaploader.h \
    painter.h \
    equalizerwindow.h \
    qcustomplot.h \
    silentrun.h \
    dialog.h

FORMS += \
        mainwindow.ui \
        settingsdialog.ui \
    inputdialog.ui \
    equalizerwindow.ui \
    dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RESOURCES += qdarkstyle/style.qrc

DISTFILES += \
    file.rc
