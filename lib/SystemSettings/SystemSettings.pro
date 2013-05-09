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
    plugin-interface.h PluginInterface \
    plugin-manager.h

private_headers = \
    debug.h \
    plugin.h

HEADERS = \
    $${public_headers} \
    $${private_headers}

SOURCES = \
    debug.cpp \
    item-base.cpp \
    plugin-manager.cpp \
    plugin.cpp

DEFINES += \
    DEBUG_ENABLED \
    PLUGIN_MANIFEST_DIR=\\\"$${INSTALL_PREFIX}/share/settings/system\\\" \
    PLUGIN_MODULE_DIR=\\\"$${INSTALL_PREFIX}/lib/system-settings\\\"

headers.files = $${public_headers}

include($${TOP_SRC_DIR}/common-installs-config.pri)
