set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")
include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/system-update)
add_definitions(-DTESTS)

# Need to get libsignon/accounts here, as they get exposed in headers
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

add_executable(tst-update-manager
    tst_updatemanager.cpp
    fakeprocess.cpp
    fakeprocess.h
    fakenetwork.cpp
    fakenetwork.h
    fakessoservice.cpp
    fakessoservice.h
    fakesystemupdate.cpp
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

target_link_libraries(tst-update-manager apt-pkg update-plugin ${UBUNTUONEAUTH_LDFLAGS})
qt5_use_modules(tst-update-manager Qml Quick Core DBus Xml Network Test)
add_test(NAME tst-update-manager COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-update-manager)

qt5_use_modules(tst-update Qml Quick Core DBus Xml Network Test)
target_link_libraries(tst-update apt-pkg update-plugin)
add_test(NAME tst-update COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-update)

add_custom_command(
    TARGET tst-update-manager
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/click.result ${CMAKE_CURRENT_BINARY_DIR}/click.result
)
