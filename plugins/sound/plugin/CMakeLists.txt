include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(sound-plugin SHARED sound-plugin.h sound-plugin.cpp)
qt5_use_modules(sound-plugin Core Qml)
target_link_libraries(sound-plugin SystemSettings)
install(TARGETS sound-plugin DESTINATION ${PLUGIN_MODULE_DIR})
