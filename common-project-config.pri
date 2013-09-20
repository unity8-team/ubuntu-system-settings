#-----------------------------------------------------------------------------
# Common configuration for all projects.
#-----------------------------------------------------------------------------

# we don't like warnings...
QMAKE_CXXFLAGS += -Werror
# Disable RTTI
QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti

TOP_SRC_DIR = $$PWD
TOP_BUILD_DIR = $${TOP_SRC_DIR}/$(BUILD_DIR)

include(coverage.pri)

#-----------------------------------------------------------------------------
# setup the installation prefix
#-----------------------------------------------------------------------------
INSTALL_PREFIX = /usr  # default installation prefix

# default prefix can be overriden by defining PREFIX when running qmake
isEmpty(PREFIX) {
    message("====")
    message("==== NOTE: To override the installation path run: `qmake PREFIX=/custom/path'")
    message("==== (current installation path is `$${INSTALL_PREFIX}')")
} else {
    INSTALL_PREFIX = $${PREFIX}
    message("====")
    message("==== install prefix set to `$${INSTALL_PREFIX}'")
}

isEmpty(LIBDIR) {
    LIBDDIR = "$${INSTALL_PREFIX}/lib"
}

PLUGIN_MANIFEST_DIR_BASE = "share/ubuntu/settings/system"
PLUGIN_MODULE_DIR_BASE="ubuntu-system-settings"
PLUGIN_PRIVATE_MODULE_DIR_BASE = "$${PLUGIN_MODULE_DIR_BASE}/private"
PLUGIN_QML_DIR_BASE="share/ubuntu/settings/system/qml-plugins"

PLUGIN_MANIFEST_DIR="$${INSTALL_PREFIX}/$${PLUGIN_MANIFEST_DIR_BASE}"
PLUGIN_MODULE_DIR="$${LIBDIR}/$${PLUGIN_MODULE_DIR_BASE}"
PLUGIN_QML_DIR="$${INSTALL_PREFIX}/$${PLUGIN_QML_DIR_BASE}"
PLUGIN_PRIVATE_MODULE_DIR="$${LIBDIR}/$${PLUGIN_PRIVATE_MODULE_DIR_BASE}"
