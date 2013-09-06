add_library(UbuntuUpdatePanel MODULE plugin.h update.h plugin.cpp update.cpp)
qt5_use_modules(UbuntuUpdatePanel Qml Quick DBus)

set(QML_SOURCES PageComponent.qml)
set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Update)
install(TARGETS UbuntuUpdatePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/sound)

install(FILES system-update.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-system-update.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icon)
