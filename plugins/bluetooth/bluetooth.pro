include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = bluetooth

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-bluetooth.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

##
## QML
##

QML_SOURCES = \
  ConfirmPasskeyDialog.qml \
  ProvidePasskeyDialog.qml \
  ProvidePinCodeDialog.qml \
  PageComponent.qml 

OTHER_FILES += $${QML_SOURCES}
qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml
QT += qml quick

##
## C++ Bits
##

# C++ bits
TARGET = UbuntuBluetoothPanel
QT += dbus
CONFIG += qt plugin no_keywords link_pkgconfig
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.Bluetooth

INCLUDEPATH += .

# Input

HEADERS += \
  agent.h \
  agentadaptor.h \
  bluetooth.h \
  device.h \
  devicemodel.h \
  killswitch.h \
  plugin.h
  
SOURCES += \
  agent.cpp \
  agentadaptor.cpp \
  bluetooth.cpp \
  device.cpp \
  devicemodel.cpp \
  plugin.cpp \
  killswitch.cpp

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
