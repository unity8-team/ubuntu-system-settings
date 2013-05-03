include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TEMPLATE = app
TARGET = system-settings

I18N_DOMAIN = system-settings

CONFIG += \
    qt

QT += \
    core \
    gui \
    qml \
    quick

HEADERS = \
    debug.h \
    i18n.h

SOURCES = \
    debug.cpp \
    i18n.cpp \
    main.cpp

QML_SOURCES = \
    qml/MainWindow.qml

RESOURCES += \
    ui.qrc

OTHER_FILES += \
    $${QML_SOURCES} \
    $${RESOURCES}

DEFINES += \
    DEBUG_ENABLED \
    I18N_DOMAIN=\\\"$${I18N_DOMAIN}\\\"

po.target = ../po/signon-ui.pot
po.depends = $${SOURCES}
po.commands = xgettext -o $@ -d $${I18N_DOMAIN} --keyword=_ $^

QMAKE_EXTRA_TARGETS += \
    po
