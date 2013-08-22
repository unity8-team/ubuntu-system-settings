include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = security-privacy

QML_SOURCES = \
    Dash.qml  \
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
