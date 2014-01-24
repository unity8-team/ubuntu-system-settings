set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")
include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/system-update)
add_definitions(-DTESTS)

find_package (PkgConfig REQUIRED)

add_executable(tst-update-manager
    tst_updatemanager.cpp
    ../../../plugins/system-update/update.cpp
    ../../../plugins/system-update/update_manager.cpp
    ../../../plugins/system-update/system_update.cpp
)

add_executable(tst-update
    tst_update.cpp
    ../../../plugins/system-update/update.cpp
)

# set the path to the library folder
include_directories(/usr/include/apt-pkg/)

target_link_libraries(tst-update-manager apt-pkg)
qt5_use_modules(tst-update-manager Qml Quick Core DBus Xml Network Test)
add_test(tst-update-manager tst-update-manager)

qt5_use_modules(tst-update Qml Quick Core DBus Xml Network Test)
target_link_libraries(tst-update apt-pkg)
add_test(tst-update tst-update)
