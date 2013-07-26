include(../../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = diagnostics

QML_SOURCES = \
    PageComponent.qml \
    DiagnosticsCheckEntry.qml

OTHER_FILES += \
    $${QML_SOURCES}

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

# C++ bits
TARGET = UbuntuDiagnostics
QT += qml quick dbus
CONFIG += qt plugin no_keywords

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Diagnostics

INCLUDEPATH += .

# Input
HEADERS += plugin.h diagnostics.h
SOURCES += plugin.cpp diagnostics.cpp

# Install path for the plugin
installPath = $${PLUGIN_PRIVATE_MODULE_DIR}/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

# find files
QMLDIR_FILE = qmldir

# make visible to qt creator
OTHER_FILES += $$QMLDIR_FILE

# create install targets for files
qmldir.path = $$installPath
qmldir.files = $$QMLDIR_FILE

INSTALLS += qmldir
