include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = appearance

QML_SOURCES = \
    ChangeImage.qml \
    CrossFadeImage.qml \
    MainImage.qml \
    MainPage.qml \
    SwappableImage.qml \
    UbuntuSwappableImage.qml

OTHER_FILES += \
    $${QML_SOURCES}

images.files = aeg.jpg greenplant.jpg
images.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += images

utilities.files = utilities.js
utilities.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += utilities

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

qml.files = $${QML_SOURCES}
qml.path = $${PLUGIN_QML_DIR}/$${TARGET}
INSTALLS += qml
