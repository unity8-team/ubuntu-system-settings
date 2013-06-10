include(../../common-project-config.pri)

TEMPLATE = lib
TARGET = SystemSettings

PLUGIN_INSTALL_BASE = $${PLUGIN_QML_DIR}/$${TARGET}

QML_SOURCES = \
    ItemPage.qml

qmldir.files = qmldir $${QML_SOURCES}
qmldir.path = $${PLUGIN_INSTALL_BASE}
INSTALLS += qmldir
