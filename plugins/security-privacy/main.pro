include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = security-privacy

QML_SOURCES = \
    Dash.qml  \
    Location.qml \
    LockSecurity.qml \
    LockWhenIdle.qml \
    PageComponent.qml \
    PhoneLocking.qml

OTHER_FILES += \
    $${QML_SOURCES}

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-security-privacy.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

# C++ bits
TARGET = UbuntuSecurityPrivacyPanel
QT += qml quick dbus
CONFIG += qt plugin no_keywords

#comment in the following line to enable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.SystemSettings.SecurityPrivacy

INCLUDEPATH += .

# Input
HEADERS += plugin.h securityprivacy.h
SOURCES += plugin.cpp securityprivacy.cpp

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

# install dbus service file
dbusdir.path = $${INSTALL_PREFIX}/share/dbus-1/interfaces
dbusdir.files = com.ubuntu.touch.AccountsService.SecurityPrivacy.xml

INSTALLS += dbusdir

ACCOUNTSSERVICE_IFACE_DIR = $$(INSTALL_ROOT)/$${INSTALL_PREFIX}/share/accountsservice/interfaces
