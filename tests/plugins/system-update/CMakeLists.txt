set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")
include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/system-update)

find_package(Qt5Test REQUIRED)
add_definitions(-DTESTS)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Install a mock click server
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/../../autopilot/ubuntu_system_settings/utils/mock_update_click_server.py
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})

# Need to get libsignon/accounts here, as they get exposed in headers
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

add_executable(tst-clickapiclient tst_clickapiclient.cpp mockclickservertestcase.h)
add_test(tst-clickapiclient tst-clickapiclient)
set_tests_properties(
    tst-clickapiclient
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009"
)
target_link_libraries(tst-clickapiclient Qt5::Test update-plugin)

add_executable(tst-clickupdatemetadata tst_clickupdatemetadata.cpp mockclickservertestcase.h)
add_test(tst-clickupdatemetadata tst-clickupdatemetadata)
set_tests_properties(
    tst-clickupdatemetadata
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009"
)
target_link_libraries(tst-clickupdatemetadata Qt5::Test update-plugin)

# add_executable(tst-update-manager
#     tst_updatemanager.cpp
#     fakeprocess.cpp
#     fakeprocess.h
#     fakenetwork.cpp
#     fakenetwork.h
#     fakessoservice.cpp
#     fakessoservice.h
#     fakesystemupdate.cpp
#     ../../../plugins/system-update/update.cpp
#     ../../../plugins/system-update/update_manager.cpp
#     ../../../plugins/system-update/system_update.cpp
# )

# add_executable(tst-update
#     tst_update.cpp
#     ../../../plugins/system-update/update.cpp
# )

# add_executable(tst-network
#     tst_network.cpp
#     ../../../plugins/system-update/update.cpp
#     ../../../plugins/system-update/network.cpp
# )

# # set the path to the library folder
# include_directories(/usr/include/apt-pkg/)

# target_link_libraries(tst-update-manager apt-pkg update-plugin ${UBUNTUONEAUTH_LDFLAGS})
# qt5_use_modules(tst-update-manager Qml Quick Core DBus Xml Network Test)
# add_test(NAME tst-update-manager COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-update-manager)

# qt5_use_modules(tst-update Qml Quick Core DBus Xml Network Test)
# target_link_libraries(tst-update apt-pkg update-plugin)
# add_test(NAME tst-update COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-update)

# qt5_use_modules(tst-network Qml Quick Core DBus Xml Network Test)
# target_link_libraries(tst-network apt-pkg update-plugin)
# add_test(NAME tst-network COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-network)
# set_tests_properties(tst-network
#     PROPERTIES ENVIRONMENT "FRAMEWORKS_FOLDER=${CMAKE_CURRENT_BINARY_DIR}")

# add_custom_command(
#     TARGET tst-update-manager
#     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/click.result ${CMAKE_CURRENT_BINARY_DIR}/click.result
# )

# add_custom_command(
#     TARGET tst-network
#     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/ubuntu-sdk-15.04.framework ${CMAKE_CURRENT_BINARY_DIR}/ubuntu-sdk-15.04.framework
# )
