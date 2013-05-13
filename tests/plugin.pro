include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TEMPLATE = lib
TARGET = test-plugin
DESTDIR = plugins

CONFIG += \
    debug \
    plugin

QT += \
    core \
    qml

LIBS += -lSystemSettings
QMAKE_LIBDIR += $${TOP_BUILD_DIR}/lib/SystemSettings
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

INCLUDEPATH += $${TOP_SRC_DIR}/lib

SOURCES += \
    test-plugin.cpp
HEADERS += \
    test-plugin.h

DEFINES += \
    DEBUG_ENABLED
