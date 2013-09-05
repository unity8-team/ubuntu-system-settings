include_directories(${BAT_DEPS_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${GLIB_INCLUDE_DIRS} ${UPOWER_GLIB_INCLUDE_DIRS})

add_library(battery-plugin SHARED battery-plugin.h battery-plugin.cpp)
qt5_use_modules(battery-plugin Core Qml)
target_link_libraries(battery-plugin SystemSettings ${GLIB_LDFLAGS} ${UPOWER_GLIB_LDFLAGS})
