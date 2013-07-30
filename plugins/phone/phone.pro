include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = phone

QML_SOURCES = \
    CallDiversion.qml \
    CallWaiting.qml \
    PageComponent.qml \
    Services.qml

OTHER_FILES += \
    $${QML_SOURCES} \
    ServiceNumber.qml

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-phone.svg
image.path = /usr/share/settings/system/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuPhonePanel
QT += qml quick dbus
CONFIG += qt plugin link_pkgconfig
PKGCONFIG += ofono-qt

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Phone

INCLUDEPATH += .

# Input
HEADERS += plugin.h phoneservices.h simservice.h
SOURCES += plugin.cpp phoneservices.cpp simservice.cpp

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
