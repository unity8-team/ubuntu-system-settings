add_subdirectory(plugin)

add_definitions(-DQT_NO_KEYWORDS)

include_directories(${GLIB_INCLUDE_DIRS} ${UPOWER_GLIB_INCLUDE_DIRS} ${LIBNM_GLIB_INCLUDE_DIRS})
add_library(UbuntuBatteryPanel MODULE plugin.h battery.h plugin.cpp battery.cpp)
qt5_use_modules(UbuntuBatteryPanel Quick Qml DBus)
target_link_libraries(UbuntuBatteryPanel ${GLIB_LDFLAGS} ${UPOWER_GLIB_LDFLAGS} ${LIBNM_GLIB_LDFLAGS})
