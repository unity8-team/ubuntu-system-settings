include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DQT_NO_KEYWORDS)

add_library(system-update-plugin SHARED system-update-plugin.h system-update-plugin.cpp)
qt5_use_modules(system-update-plugin Core)
target_link_libraries(system-update-plugin SystemSettings)
install(TARGETS system-update-plugin DESTINATION ${PLUGIN_MODULE_DIR})
