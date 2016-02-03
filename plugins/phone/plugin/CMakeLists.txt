include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(phone-plugin SHARED phone-plugin.h phone-plugin.cpp)
qt5_use_modules(phone-plugin Core Qml DBus)
target_link_libraries(phone-plugin SystemSettings)
install(TARGETS phone-plugin DESTINATION ${PLUGIN_MODULE_DIR})
