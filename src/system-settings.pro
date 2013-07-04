include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TEMPLATE = app
TARGET = system-settings

I18N_DOMAIN = ubuntu-system-settings

CONFIG += \
    qt

QT += \
    core \
    gui \
    qml \
    quick

LIBS += -lSystemSettings
QMAKE_LIBDIR += $${TOP_BUILD_DIR}/lib/SystemSettings
INCLUDEPATH += $${TOP_SRC_DIR}/lib

HEADERS = \
    debug.h \
    i18n.h \
    item-model.h \
    plugin-manager.h \
    plugin.h

SOURCES = \
    debug.cpp \
    i18n.cpp \
    item-model.cpp \
    main.cpp \
    plugin-manager.cpp \
    plugin.cpp

QML_SOURCES = \
    qml/CategoryGrid.qml \
    qml/EntryComponent.qml \
    qml/MainWindow.qml \
    qml/UncategorizedItemsView.qml

RESOURCES += \
    ui.qrc

OTHER_FILES += \
    $${QML_SOURCES} \
    $${RESOURCES}

DEFINES += \
    DEBUG_ENABLED \
    I18N_DOMAIN=\\\"$${I18N_DOMAIN}\\\" \
    PLUGIN_MANIFEST_DIR=\\\"$${PLUGIN_MANIFEST_DIR}\\\" \
    PLUGIN_MODULE_DIR=\\\"$${PLUGIN_MODULE_DIR}\\\" \
    PLUGIN_PRIVATE_MODULE_DIR=\\\"$${PLUGIN_PRIVATE_MODULE_DIR}\\\" \
    PLUGIN_QML_DIR=\\\"$${PLUGIN_QML_DIR}\\\"

po.target = ../po/system-settings.pot
po.depends = $${SOURCES}
po.commands = xgettext -o $@ -d $${I18N_DOMAIN} --keyword=_ $^

QMAKE_EXTRA_TARGETS += \
    po
