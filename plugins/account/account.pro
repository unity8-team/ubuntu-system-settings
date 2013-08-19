include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = account

#QML_SOURCES = \

#OTHER_FILES += \
#    $${QML_SOURCES}

#settings.files = $${TARGET}.settings
#settings.path = $${PLUGIN_MANIFEST_DIR}
#INSTALLS += settings

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

# C++ bits
TARGET = UbuntuAccountPlugin
QT += qml quick dbus
CONFIG += qt plugin no_keywords

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Account

INCLUDEPATH += .

# Input
HEADERS += plugin.h account.h
SOURCES += plugin.cpp account.cpp

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
