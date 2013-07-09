include(../../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TEMPLATE = lib
TARGET = security-privacy

settings.files = $${TARGET}.settings
settings.path = $${PLUGIN_MANIFEST_DIR}
INSTALLS += settings

image.files = settings-security-privacy.svg
image.path = $$INSTALL_PREFIX/share/settings/system/icons
INSTALLS += image
