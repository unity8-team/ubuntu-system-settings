include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(notifications-plugin SHARED notifications-plugin.h notifications-plugin.cpp)
qt5_use_modules(notifications-plugin Core Qml)
target_link_libraries(notifications-plugin SystemSettings)
install(TARGETS notifications-plugin DESTINATION ${PLUGIN_MODULE_DIR})
