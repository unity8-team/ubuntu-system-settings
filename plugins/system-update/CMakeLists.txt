set(QML_SOURCES PageComponent.qml Configuration.qml)
set(JS_SOURCES js/utils.js)

add_library(UbuntuUpdatePanel MODULE plugin.h system_update.h update_manager.h update.h plugin.cpp system_update.cpp update_manager.cpp update.cpp
${QML_SOURCES})
qt5_use_modules(UbuntuUpdatePanel Qml Quick DBus)
include_directories(/usr/include/apt-pkg/)
target_link_libraries(UbuntuUpdatePanel apt-pkg)

find_package (PkgConfig REQUIRED)
set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Update)
install(TARGETS UbuntuUpdatePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/system-update)
install(FILES ${JS_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/system-update/js)

install(FILES system-update.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES images/settings-system-update.svg images/distributor-logo.png DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
