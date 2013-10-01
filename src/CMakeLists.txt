add_definitions(-DI18N_DOMAIN="ubuntu-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${PLUGIN_MANIFEST_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${PLUGIN_QML_DIR}")
add_definitions(-DPLUGIN_MODULE_DIR="${PLUGIN_MODULE_DIR}")

add_subdirectory(SystemSettings)

set(USS_SOURCES 
    debug.cpp
    i18n.cpp
    item-model.cpp
    main.cpp
    plugin-manager.cpp
    plugin.cpp
    utils.cpp
)

QT5_ADD_RESOURCES(system-settings-resources ui.qrc)

add_executable(system-settings ${USS_SOURCES} ${system-settings-resources})
qt5_use_modules(system-settings Core Gui Quick Qml)
target_link_libraries(system-settings SystemSettings)
install(TARGETS system-settings RUNTIME DESTINATION bin)

add_custom_target(po COMMAND
${XGETTEXT_BIN} -o ${CMAKE_SOURCE_DIR}/po/ubuntu-system-settings.pot -d ubuntu-system-settings
--keyword=_ ${USS_SOURCES}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
