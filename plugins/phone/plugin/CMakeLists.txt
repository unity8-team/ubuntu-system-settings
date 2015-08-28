include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QOFONO_INCLUDE_DIRS})

add_library(phone-plugin SHARED phone-plugin.h phone-plugin.cpp)
qt5_use_modules(phone-plugin Core Qml DBus)
target_link_libraries(phone-plugin SystemSettings ${QOFONO_LDFLAGS})
install(TARGETS phone-plugin DESTINATION ${PLUGIN_MODULE_DIR})
