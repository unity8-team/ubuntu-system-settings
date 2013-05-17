include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = qmlutils

API_URI = "Ubuntu.SystemSettings.Utils"

DESTDIR = $$replace(API_URI, \\., /)

CONFIG += \
    qt \
    plugin

QT += \
    core \
    qml

# Error on undefined symbols
QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

HEADERS = \
    plugin.h \
    qqmlflatmodel_p.h \
    qqmlflatmodel_p_p.h

SOURCES = \
    plugin.cpp \
    qqmlflatmodel.cpp

DEFINES += API_URI=\\\"$${API_URI}\\\"

QMLDIR_FILES += qmldir
QMAKE_SUBSTITUTES += qmldir.in
OTHER_FILES += qmldir.in

copy2build.output = $${DESTDIR}/${QMAKE_FILE_IN}
copy2build.input = QMLDIR_FILES
copy2build.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy2build.name = COPY ${QMAKE_FILE_IN}
copy2build.variable_out = PRE_TARGETDEPS
copy2build.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copy2build

PLUGIN_INSTALL_BASE = $$[QT_INSTALL_QML]/$$replace(API_URI, \\., /)
target.path = $${PLUGIN_INSTALL_BASE}
INSTALLS += target

qmldir.files = qmldir
qmldir.path = $${PLUGIN_INSTALL_BASE}
INSTALLS += qmldir
