include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QOFONO_INCLUDE_DIRS})

add_library(cellular-plugin SHARED cellular-plugin.h cellular-plugin.cpp)
qt5_use_modules(cellular-plugin Core Qml DBus)
target_link_libraries(cellular-plugin SystemSettings ${QOFONO_LDFLAGS})
install(TARGETS cellular-plugin DESTINATION ${PLUGIN_MODULE_DIR})
