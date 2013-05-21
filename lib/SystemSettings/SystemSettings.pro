include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

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

public_headers = \
    item-base.h ItemBase \
    plugin-interface.h PluginInterface

private_headers = \
    debug.h

HEADERS = \
    $${public_headers} \
    $${private_headers}

SOURCES = \
    item-base.cpp

DEFINES += \
    DEBUG_ENABLED

headers.files = $${public_headers}

target.path = $${PLUGIN_MODULE_DIR}

include($${TOP_SRC_DIR}/common-installs-config.pri)
