#-------------------------------------------------
#
# Project created by QtCreator 2010-06-23T12:55:35
#
#-------------------------------------------------

QT       += core gui

TARGET = cube
TEMPLATE = app

SOURCES += main.cpp \
    sensors.cpp

contains(QT_CONFIG, opengl) {
    message(Building with OpenGL support.)
    QT += opengl
    CONFIG += mobility
    MOBILITY += sensors

    SOURCES += mainwidget.cpp \
       geometryengine.cpp

    HEADERS += \
        mainwidget.h \
        geometryengine.h

    RESOURCES += \
        shaders.qrc \
        textures.qrc

    OTHER_FILES += \
        vshader.glsl \
        fshader.glsl
} else {
    message(OpenGL support is not available.)
}

symbian: include($$PWD/../../symbianpkgrules.pri)
maemo5: include($$PWD/../../maemo5pkgrules.pri)
contains(MEEGO_EDITION,harmattan): include($$PWD/../../harmattanpkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
simulator: warning(This example might not fully work on Simulator platform)

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog


contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/cube/bin
    INSTALLS += target
}

HEADERS += \
    sensors.h



contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = cube.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile
}

contains(MEEGO_EDITION,harmattan) {
    icon.files = icon.png
    icon.path = /usr/share/icons/hicolor/80x80/apps
    INSTALLS += icon
}
