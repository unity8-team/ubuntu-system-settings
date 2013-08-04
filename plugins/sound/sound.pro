include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = sound

QML_SOURCES = \
    PageComponent.qml \
    SilentModeWarning.qml \
    SoundsList.qml

OTHER_FILES += \
    $${QML_SOURCES}

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

utilities.files = utilities.js
utilities.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += utilities

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-sounds.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuSoundPanel
QT += qml quick
CONFIG += qt plugin no_keywords

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Sound

INCLUDEPATH += .

# Input
HEADERS += plugin.h sound.h
SOURCES += plugin.cpp sound.cpp

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
