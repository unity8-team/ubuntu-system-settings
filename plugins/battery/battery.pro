include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = battery

QML_SOURCES = \
    PageComponent.qml \
    SleepValues.qml

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

image.files = settings-battery.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

