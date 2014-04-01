add_definitions(-DI18N_DOMAIN="ubuntu-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${PLUGIN_MANIFEST_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${PLUGIN_QML_DIR}")
add_definitions(-DPLUGIN_MODULE_DIR="${PLUGIN_MODULE_DIR}")

execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=plugindir unity-shell-api OUTPUT_VARIABLE SHELL_PLUGINDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
if(SHELL_PLUGINDIR STREQUAL "")
    message(FATAL_ERROR "Could not determine plugin import dir.")
endif()

add_definitions(-DSHELL_PLUGINDIR="${SHELL_PLUGINDIR}")

set(WIZARD_ROOT "${CMAKE_INSTALL_PREFIX}/share/ubuntu/settings/wizard")
add_definitions(-DWIZARD_ROOT="${WIZARD_ROOT}")

file(GLOB_RECURSE QML_FILES
    qml/*
)

set(WIZARD_SOURCES
    main.cpp
    PageList.cpp
)

add_executable(system-settings-wizard
    ${WIZARD_SOURCES}
    ${system-settings-wizard-resources}
    ${QML_FILES} # This is to make qml and image files appear in the IDE's project tree
)

qt5_use_modules(system-settings-wizard Core Gui Quick Qml)

add_subdirectory(Utils)

install(TARGETS system-settings-wizard RUNTIME DESTINATION bin)
install(FILES ubuntu-system-settings-wizard.conf DESTINATION share/upstart)

install(DIRECTORY qml DESTINATION ${WIZARD_ROOT})
