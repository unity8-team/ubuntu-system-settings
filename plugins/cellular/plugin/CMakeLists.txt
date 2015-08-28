include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DQT_NO_KEYWORDS)

add_library(cellular-plugin SHARED cellular-plugin.h cellular-plugin.cpp)
qt5_use_modules(cellular-plugin Core Qml DBus)
target_link_libraries(cellular-plugin SystemSettings)
install(TARGETS cellular-plugin DESTINATION ${PLUGIN_MODULE_DIR})
