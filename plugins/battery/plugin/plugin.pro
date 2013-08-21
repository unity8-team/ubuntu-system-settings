include(../../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = battery-plugin

CONFIG += \
    qt \
    plugin \
    no_keywords \
    link_pkgconfig

PKGCONFIG += glib-2.0 upower-glib

QT += \
    core \
    qml

LIBS += -lSystemSettings
QMAKE_LIBDIR += $${TOP_BUILD_DIR}/lib/SystemSettings

INCLUDEPATH += $${TOP_SRC_DIR}/lib

SOURCES += \
    battery-plugin.cpp
HEADERS += \
    battery-plugin.h

OTHER_FILES += \
    $${QML_SOURCES} \
    $${RESOURCES}

target.path = $${PLUGIN_MODULE_DIR}
INSTALLS += target
