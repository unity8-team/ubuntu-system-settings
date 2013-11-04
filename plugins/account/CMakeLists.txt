add_library(UbuntuAccountPanel MODULE
plugin.cpp account.cpp plugin.h account.h)
qt5_use_modules(UbuntuAccountPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Account)
install(TARGETS UbuntuAccountPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES settings-account.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES account.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
