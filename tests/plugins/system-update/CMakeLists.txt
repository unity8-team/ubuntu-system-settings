set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")
add_definitions(-DTESTS)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/system-update
    /usr/include/apt-pkg/)

find_package(Qt5Test REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)

pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PLUGIN_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/system-update/clickapiclient.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/clicktokendownloader.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/clickupdatemanager.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/helpers.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatemodel.cpp
)

set(PLUGIN_LIBS
    Qt5::Sql
    Qt5::Network
    apt-pkg
    ${UBUNTUONEAUTH_LDFLAGS}
)

# Install a mock click server
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/../../autopilot/ubuntu_system_settings/utils/mock_update_click_server.py
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})

# Install a mock click command
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/../../autopilot/ubuntu_system_settings/utils/mock_click_command
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})

add_executable(tst-clickapiclient
               tst_clickapiclient.cpp
               mockclickservertestcase.h
               ${PLUGIN_SOURCES})
add_test(tst-clickapiclient tst-clickapiclient)
set_tests_properties(
    tst-clickapiclient
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata"
)
target_link_libraries(tst-clickapiclient Qt5::Test ${PLUGIN_LIBS})

add_executable(tst-clicktokendownloader
               tst_clicktokendownloader.cpp
               mockclickservertestcase.h
               ${PLUGIN_SOURCES})
add_test(tst-clicktokendownloader tst-clicktokendownloader)
set_tests_properties(
    tst-clicktokendownloader
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata"
)
target_link_libraries(tst-clicktokendownloader Qt5::Test ${PLUGIN_LIBS})

# add_executable(tst-clickupdatemanager
#                tst_clickupdatemanager.cpp
#                mockclickservertestcase.h
#                ${PLUGIN_SOURCES})
# add_test(tst-clickupdatemanager tst-clickupdatemanager)
# set_tests_properties(
#     tst-clickupdatemanager
#         PROPERTIES
#         ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata;CLICK_COMMAND=${CMAKE_CURRENT_BINARY_DIR}/mock_click_command"
# )
# target_link_libraries(tst-clickupdatemanager Qt5::Test ${PLUGIN_LIBS})

add_executable(tst-updatedb
               tst_updatedb.cpp
               ${PLUGIN_SOURCES})
add_test(tst-updatedb tst-updatedb)
target_link_libraries(tst-updatedb Qt5::Test ${PLUGIN_LIBS})

add_executable(tst-update
               tst_update.cpp
               ${PLUGIN_SOURCES})
add_test(tst-update tst-update)
target_link_libraries(tst-update Qt5::Test ${PLUGIN_LIBS})

add_executable(tst-updatemodel
               tst_updatemodel.cpp
               ${PLUGIN_SOURCES})
add_test(tst-updatemodel tst-updatemodel)
target_link_libraries(tst-updatemodel Qt5::Test ${PLUGIN_LIBS})

add_executable(tst-updatepluginhelpers
               tst_helpers.cpp
               ${PLUGIN_SOURCES})
add_test(tst-updatepluginhelpers tst-updatepluginhelpers)
set_tests_properties(
    tst-updatepluginhelpers
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009;FRAMEWORKS_FOLDER=${CMAKE_CURRENT_BINARY_DIR};CLICK_TOKEN_URL=http://example.org;"
)
target_link_libraries(tst-updatepluginhelpers Qt5::Test ${PLUGIN_LIBS})

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
