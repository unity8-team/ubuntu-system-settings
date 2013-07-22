include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib

schema.path = $${INSTALL_PREFIX}/share/glib-2.0/schemas
schema.files += com.ubuntu.touch.system-settings.gschema.xml
INSTALLS += schema
