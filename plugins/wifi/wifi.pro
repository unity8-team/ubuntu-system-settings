include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = wifi

QML_SOURCES = \
    AccessPoint.qml \
    BaseMenuItem.qml \
    BasicMenuItem.qml \
    DivMenuItem.qml \
    FramedMenuItem.qml \
    HLine.qml \
    MenuItem.qml \
    MenuItemFactory.qml \
    PageComponent.qml \
    RemoveBackground.qml \
    SectionMenuItem.qml \
    StandardMenuItem.qml \
    SwitchMenuItem.qml

OTHER_FILES += \
    $${QML_SOURCES} \
    AccessPoint.qml \
    BaseMenuItem.qml \
    BasicMenuItem.qml \
    DivMenuItem.qml \
    FramedMenuItem.qml \
    HLine.qml \
    MenuItem.qml \
    MenuItemFactory.qml \
    PageComponent.qml \
    RemoveBackground.qml \
    SectionMenuItem.qml \
    StandardMenuItem.qml \
    SwitchMenuItem.qml

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-wifi.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuWifiPanel
QT += qml quick dbus
CONFIG += qt plugin no_keywords

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Wifi

SOURCES += \
    modelprinter.cpp \
    plugin.cpp

HEADERS += \
    modelprinter.h \
    plugin.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += qmenumodel

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
