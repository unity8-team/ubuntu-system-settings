include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TARGET = tst_arguments

CONFIG += \
    debug

QT += \
    core \
    qml \
    testlib

SRC_DIR = $$TOP_SRC_DIR/src
SOURCES += \
    tst_arguments.cpp \
    $$SRC_DIR/utils.cpp
HEADERS += \
    $$SRC_DIR/utils.cpp

INCLUDEPATH += \
    . \
    $$SRC_DIR

DEFINES += \
    DEBUG_ENABLED \
    UNIT_TESTS

check.commands = "xvfb-run -a ./$$TARGET"
check.depends = $$TARGET
QMAKE_EXTRA_TARGETS += check
