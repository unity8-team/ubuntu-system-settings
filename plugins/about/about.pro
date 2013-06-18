include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = about

QML_SOURCES = \
    EntryComponent.qml \
    PageComponent.qml \
    Storage.qml \
    StorageBar.qml

OTHER_FILES += \
    $${QML_SOURCES}

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

image.files = settings-about.png
image.path = /usr/share/settings/system/icons
INSTALLS += image
