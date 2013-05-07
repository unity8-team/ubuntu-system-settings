include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TARGET = tst_plugins

CONFIG += \
    debug \
    qtestlib

QT += \
    core

SOURCES += \
    tst_plugins.cpp \
    $$TOP_SRC_DIR/src/plugin-manager.cpp \
    $$TOP_SRC_DIR/src/plugin.cpp
HEADERS += \
    $$TOP_SRC_DIR/src/debug.h \
    $$TOP_SRC_DIR/src/plugin-manager.h \
    $$TOP_SRC_DIR/src/plugin.h

INCLUDEPATH += \
    . \
    $$TOP_SRC_DIR/src

DEFINES += \
    DEBUG_ENABLED \
    PLUGIN_BASE_DIR=\\\"$${TOP_SRC_DIR}/tests/data\\\" \
    UNIT_TESTS

check.commands = "xvfb-run -a ./$$TARGET"
check.depends = $$TARGET
QMAKE_EXTRA_TARGETS += check
