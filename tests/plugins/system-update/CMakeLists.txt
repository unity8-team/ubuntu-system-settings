include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/system-update)
add_definitions(-DTESTS)

# Need to get libsignon/accounts here, as they get exposed in headers
find_package (PkgConfig REQUIRED)

add_executable(tst-update
    tst_update.cpp
    ../../../plugins/system-update/update.cpp
)

# set the path to the library folder
include_directories(/usr/include/apt-pkg/)

qt5_use_modules(tst-update Qml Quick Core DBus Xml Network Test)
target_link_libraries(tst-update apt-pkg)
add_test(tst-update tst-update)
