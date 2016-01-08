include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DQT_NO_KEYWORDS)

add_library(mouse-plugin SHARED mouse-plugin.h mouse-plugin.cpp)
qt5_use_modules(mouse-plugin Core Qml DBus)
target_link_libraries(mouse-plugin SystemSettings)
install(TARGETS mouse-plugin DESTINATION ${PLUGIN_MODULE_DIR})
