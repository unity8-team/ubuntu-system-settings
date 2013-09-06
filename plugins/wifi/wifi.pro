include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = wifi

QML_SOURCES = \
    PageComponent.qml

OTHER_FILES += \
    $${QML_SOURCES} \
    PageComponent.qml

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-wifi.svg
image.path = $${PLUGIN_MANIFEST_DIR}/icons
INSTALLS += image

SOURCES += \
    modelprinter.cpp

HEADERS += \
    modelprinter.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += qmenumodel
