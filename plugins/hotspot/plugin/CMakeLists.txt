include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DQT_NO_KEYWORDS)

add_library(hotspot-plugin SHARED hotspot-plugin.h hotspot-plugin.cpp)
qt5_use_modules(hotspot-plugin Core Qml DBus)
target_link_libraries(hotspot-plugin SystemSettings)
install(TARGETS hotspot-plugin DESTINATION ${PLUGIN_MODULE_DIR})
