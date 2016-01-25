# This is a temporary snapshot of the WIP QInputInfo API as we
# require this in unity now but upstream isn't finished yet.
# Eventually this should be dropped in favor of the upstream
# QInputInfo API.

project(InputInfo)

find_package(Qt5Core REQUIRED)
find_package(Qt5Quick REQUIRED)

pkg_check_modules(LIBUDEV REQUIRED libudev)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIBUDEV_INCLUDE_DIRS}
    ${LIBEVDEV_INCLUDE_DIRS}
)

set(InputInfo_SOURCES
    plugin.cpp
    qinputinfo.cpp
    qdeclarativeinputdeviceinfo.cpp
    linux/qinputdeviceinfo_linux.cpp
)

add_library(InputInfo SHARED
    ${InputInfo_SOURCES}
)

target_link_libraries(InputInfo
    ${LIBUDEV_LDFLAGS}
    ${LIBEVDEV_LDFLAGS}
)

qt5_use_modules(InputInfo Core Qml Quick)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Mouse/InputInfo)
install(TARGETS InputInfo DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
