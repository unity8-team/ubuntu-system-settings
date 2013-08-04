include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = example-plugin

CONFIG += \
    debug \
    plugin

QT += \
    core \
    qml

LIBS += -lSystemSettings
QMAKE_LIBDIR += $${TOP_BUILD_DIR}/lib/SystemSettings

INCLUDEPATH += $${TOP_SRC_DIR}/lib

SOURCES += \
    example-plugin.cpp
HEADERS += \
    example-plugin.h

QML_SOURCES = \
    PageComponent.qml

RESOURCES += \
    ui.qrc

OTHER_FILES += \
    $${QML_SOURCES} \
    $${RESOURCES}

target.path = $${PLUGIN_MODULE_DIR}
INSTALLS += target

settings.files = example.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings
