include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TEMPLATE = lib
TARGET = SystemSettings

CONFIG += \
    qt

QT += \
    core \
    gui \
    qml \
    quick

# Error on undefined symbols
QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

HEADERS = \
    debug.h \
    plugin-base.h \
    plugin-manager.h \
    plugin.h

SOURCES = \
    debug.cpp \
    plugin-base.cpp \
    plugin-manager.cpp \
    plugin.cpp

DEFINES += \
    DEBUG_ENABLED \
    PLUGIN_BASE_DIR=\\\"$${INSTALL_PREFIX}/share/settings/system\\\"
