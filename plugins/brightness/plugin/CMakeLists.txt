include_directories(${CMAKE_CURRENT_BINARY_DIR} ${Qt5Gui_PRIVATE_INCLUDE_DIRS} ${MIR_INCLUDE_DIRS})

add_definitions(-DQT_NO_KEYWORDS)

add_library(brightness-plugin SHARED brightness-plugin.h brightness-plugin.cpp)
qt5_use_modules(brightness-plugin Core Qml DBus)
target_link_libraries(brightness-plugin SystemSettings uss-displays ${Qt5Gui_PRIVATE_LDFLAGS} ${MIR_LDFLAGS})
install(TARGETS brightness-plugin DESTINATION ${PLUGIN_MODULE_DIR})
