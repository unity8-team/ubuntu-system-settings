include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = language

QML_SOURCES = DisplayLanguage.qml \
              KeyboardLayouts.qml \
              PageComponent.qml   \
              SpellChecking.qml

OTHER_FILES += $${QML_SOURCES}

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

image.files = settings-language.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuLanguagePlugin
QT += qml quick dbus xmlpatterns
CONFIG += qt link_pkgconfig plugin no_keywords

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.LanguagePlugin

INCLUDEPATH += .
PKGCONFIG += accountsservice glib-2.0

# Input
HEADERS += keyboard-layout.h language-plugin.h plugin.h subset-model.h
SOURCES += keyboard-layout.cpp language-plugin.cpp plugin.cpp subset-model.cpp

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
