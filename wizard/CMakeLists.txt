add_definitions(-DI18N_DOMAIN="ubuntu-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${PLUGIN_MANIFEST_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${PLUGIN_QML_DIR}")
add_definitions(-DPLUGIN_MODULE_DIR="${PLUGIN_MODULE_DIR}")

set(WIZARD_SOURCES
    main.cpp
)

QT5_ADD_RESOURCES(system-settings-wizard-resources ui.qrc)

add_executable(system-settings-wizard ${WIZARD_SOURCES} ${system-settings-wizard-resources})

qt5_use_modules(system-settings-wizard Core Gui Quick Qml)

install(TARGETS system-settings-wizard RUNTIME DESTINATION bin)
