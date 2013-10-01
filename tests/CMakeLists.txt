include_directories(${CMAKE_CURRENT_BINARY_DIR} ../src)
add_definitions(-DI18N_DOMAIN="ubuntu-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DPLUGIN_MODULE_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_library(test-plugin SHARED test-plugin.cpp test-plugin.h)
target_link_librarieS(test-plugin SystemSettings)
qt5_use_modules(test-plugin Core Qml)

add_executable(tst-plugins 
    tst_plugins.cpp
    ../src/debug.cpp
    ../src/item-model.cpp
    ../src/plugin-manager.cpp
    ../src/plugin.cpp
    ../src/debug.h
    ../src/item-model.h
    ../src/plugin-manager.h
    ../src/plugin.h
)

add_executable(tst-arguments
    tst_arguments.cpp
    ../src/utils.cpp
)

qt5_use_modules(tst-plugins Core Qml Test)
target_link_libraries(tst-plugins SystemSettings)
add_test(tst-plugins tst-plugins)

qt5_use_modules(tst-arguments Core Test)
add_test(tst-arguments tst-arguments)
