include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = phone

QML_SOURCES = \
    CallDiversion.qml \
    CallWaiting.qml \
    PageComponent.qml \
    ServiceInfo.qml   \
    Services.qml

OTHER_FILES += \
    $${QML_SOURCES}

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-phone.svg
image.path = /usr/share/settings/system/icons
INSTALLS += image
