include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TARGET = tst_plugins

CONFIG += \
    debug

QT += \
    core \
    qml \
    testlib

LIBS += -lSystemSettings
QMAKE_LIBDIR += $${TOP_BUILD_DIR}/lib/SystemSettings
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

INCLUDEPATH += $${TOP_SRC_DIR}/lib

SRC_DIR = $$TOP_SRC_DIR/src
SOURCES += \
    tst_plugins.cpp \
    $$SRC_DIR/debug.cpp \
    $$SRC_DIR/item-model.cpp \
    $$SRC_DIR/plugin-manager.cpp \
    $$SRC_DIR/plugin.cpp
HEADERS += \
    $$SRC_DIR/debug.h \
    $$SRC_DIR/item-model.h \
    $$SRC_DIR/plugin-manager.h \
    $$SRC_DIR/plugin.h

INCLUDEPATH += \
    . \
    $$SRC_DIR

DEFINES += \
    DEBUG_ENABLED \
    PLUGIN_MANIFEST_DIR=\\\"$${TOP_SRC_DIR}/tests/data\\\" \
    PLUGIN_MODULE_DIR=\\\"$${TOP_BUILD_DIR}/tests/plugins\\\" \
    PLUGIN_QML_DIR=\\\"$${TOP_BUILD_DIR}/tests/plugins\\\" \
    UNIT_TESTS

check.commands = "xvfb-run -a ./$$TARGET"
check.depends = $$TARGET
QMAKE_EXTRA_TARGETS += check
