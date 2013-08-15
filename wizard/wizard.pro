include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TEMPLATE = app

CONFIG += \
    qt

QT += \
    core \
    gui \
    qml \
    quick

SOURCES += main.cpp

QML_SOURCES = \
    qml/main.qml \
    qml/Components/Page.qml \
    qml/Pages/AboutYouPage.qml \
    qml/Pages/LocationPage.qml \
    qml/Pages/SetupCompletePage.qml \
    qml/Pages/SimCardPage.qml \
    qml/Pages/WelcomePage.qml \
    qml/Pages/WiFiPage.qml

RESOURCES += \
    ui.qrc

OTHER_FILES += \
    $${QML_SOURCES} \
    $${RESOURCES}

DEFINES += \
    PLUGIN_PRIVATE_MODULE_DIR=\\\"$${PLUGIN_PRIVATE_MODULE_DIR}\\\" \
    PLUGIN_QML_DIR=\\\"$${PLUGIN_QML_DIR}\\\"
