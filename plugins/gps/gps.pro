include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = gps

QML_SOURCES = \
    EntryComponent.qml

OTHER_FILES += \
    $${QML_SOURCES}

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

image.files = settings-gps.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

