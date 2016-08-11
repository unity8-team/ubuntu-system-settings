include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DQT_NO_KEYWORDS)

add_library(reset-plugin SHARED reset-plugin.h reset-plugin.cpp)
qt5_use_modules(reset-plugin Core Qml)
target_link_libraries(reset-plugin SystemSettings)
install(TARGETS reset-plugin DESTINATION ${PLUGIN_MODULE_DIR})
