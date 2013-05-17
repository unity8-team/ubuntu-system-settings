include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = default

CONFIG += \
    debug \
    plugin

QT += \
    core \
    qml

LIBS += -lSystemSettings
QMAKE_LIBDIR += $${TOP_BUILD_DIR}/lib/SystemSettings
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

INCLUDEPATH += $${TOP_SRC_DIR}/lib

SOURCES += \
    default-plugin.cpp
HEADERS += \
    default-plugin.h

QML_SOURCES = \
    PageComponent.qml

RESOURCES += \
    ui.qrc

OTHER_FILES += \
    $${QML_SOURCES} \
    $${RESOURCES}

target.path = $${INSTALL_PREFIX}/lib/system-settings
INSTALLS += target

settings.files = default-example.settings
settings.path = $${INSTALL_PREFIX}/share/settings/system
INSTALLS += settings
