include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = background

QML_SOURCES = \
    ChangeImage.qml \
    MainPage.qml \
    SwappableImage.qml \
    UbuntuSwappableImage.qml \
    OverlayImage.qml

OTHER_FILES += \
    $${QML_SOURCES}

images.files = \
    aeg.jpg \
    darkeningclockwork.jpg \
    homeoverlay.svg \
    welcomeoverlay.svg
images.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += images

utilities.files = utilities.js
utilities.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += utilities

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

image.files = settings-background.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuBackgroundPanel
QT += qml quick dbus
CONFIG += qt plugin no_keywords

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Background

INCLUDEPATH += .

# Input
HEADERS += plugin.h background.h
SOURCES += plugin.cpp background.cpp

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
