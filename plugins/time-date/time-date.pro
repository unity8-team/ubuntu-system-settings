include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = time-date

QML_SOURCES = \
    ChooseTimeZone.qml \
    PageComponent.qml

OTHER_FILES += \
    $${QML_SOURCES}

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-time-date.svg
image.path = /usr/share/settings/system/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuTimeDatePanel
QT += qml quick dbus
CONFIG += qt plugin no_keywords link_pkgconfig

PKGCONFIG += timezonemap glib-2.0

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.TimeDate

INCLUDEPATH += .

# Input
HEADERS += plugin.h timedate.h
SOURCES += plugin.cpp timedate.cpp

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
