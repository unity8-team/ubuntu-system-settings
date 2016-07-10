add_definitions(-DTESTS)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/system-update
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click

    ${CMAKE_CURRENT_SOURCE_DIR}/mocks

    /usr/include/apt-pkg/
)

find_package(Qt5Test REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)

pkg_check_modules(UAL REQUIRED ubuntu-app-launch-2)
add_definitions(${UAL_CFLAGS} ${UAL_CFLAGS_OTHER})

pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PLUGIN_LIBS
    Qt5::Test
    Qt5::Sql
    Qt5::Network
    apt-pkg
    ${UAL_LDFLAGS}
    ${UBUNTUONEAUTH_LDFLAGS}
)

# Install a mock click server
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/mockclickserver.py
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})

# # Install a mock click command
# file(COPY
#     ${CMAKE_CURRENT_SOURCE_DIR}/mockclickcommand
#     DESTINATION
#     ${CMAKE_CURRENT_BINARY_DIR})

add_executable(tst-systemupdate-click_client
    tst_click_client.cpp

    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager.h

    ${CMAKE_SOURCE_DIR}/plugins/system-update/helpers.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager_impl.cpp
)
add_test(tst-systemupdate-click_client tst-systemupdate-click_client)
set_tests_properties(
    tst-systemupdate-click_client
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata"
)
target_link_libraries(tst-systemupdate-click_client ${PLUGIN_LIBS})

add_executable(tst-systemupdate-click_token_downloader
    tst_click_token_downloader.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader_impl.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager.h

    ${CMAKE_SOURCE_DIR}/plugins/system-update/helpers.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager_impl.cpp

)
add_test(tst-systemupdate-click_token_downloader tst-systemupdate-click_token_downloader)
set_tests_properties(
    tst-systemupdate-click_token_downloader
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata"
)
target_link_libraries(tst-systemupdate-click_token_downloader ${PLUGIN_LIBS})

add_executable(tst-systemupdate-click_update_manager
    tst_click_update_manager.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/manifest.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/sso.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader_factory.h

    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/manifest_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/sso_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader_factory_impl.cpp

    ${CMAKE_SOURCE_DIR}/plugins/system-update/clickupdatemanager.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/helpers.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp

    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatemodel.cpp
)
add_test(tst-systemupdate-click_update_manager tst-systemupdate-click_update_manager)
set_tests_properties(
    tst-systemupdate-click_update_manager
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1"
)
target_link_libraries(tst-systemupdate-click_update_manager ${PLUGIN_LIBS})

add_executable(tst-systemupdate-system_update
    tst_system_update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager.h

    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager_impl.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp
)
add_test(tst-systemupdate-system_update tst-systemupdate-system_update)
target_link_libraries(tst-systemupdate-system_update ${PLUGIN_LIBS})

add_executable(tst-systemupdate-updatedb
    tst_updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
)
add_test(tst-systemupdate-updatedb tst-systemupdate-updatedb)
target_link_libraries(tst-systemupdate-updatedb ${PLUGIN_LIBS})

add_executable(tst-systemupdate-update
    tst_update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
)
add_test(tst-systemupdate-update tst-systemupdate-update)
target_link_libraries(tst-systemupdate-update ${PLUGIN_LIBS})

add_executable(tst-systemupdate-updatemodel
    tst_updatemodel.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager.h

    ${CMAKE_SOURCE_DIR}/plugins/system-update/update.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatedb.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/updatemodel.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/systemupdate.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager_impl.cpp
)
add_test(tst-systemupdate-updatemodel tst-systemupdate-updatemodel)
target_link_libraries(tst-systemupdate-updatemodel ${PLUGIN_LIBS})

add_executable(tst-systemupdate-updatepluginhelpers
    tst_helpers.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/helpers.cpp
)
add_test(tst-systemupdate-updatepluginhelpers tst-systemupdate-updatepluginhelpers)
set_tests_properties(
    tst-systemupdate-updatepluginhelpers
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009;FRAMEWORKS_FOLDER=${CMAKE_CURRENT_BINARY_DIR};CLICK_TOKEN_URL=http://example.org;"
)
target_link_libraries(tst-systemupdate-updatepluginhelpers ${PLUGIN_LIBS})

add_executable(tst-systemupdate-click_manifest
    tst_click_manifest.cpp

    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/manifest.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/helpers.cpp
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/manifest_impl.cpp
)
add_test(tst-systemupdate-click_manifest tst-systemupdate-click_manifest)
set_tests_properties(
    tst-systemupdate-click_manifest
        PROPERTIES
        ENVIRONMENT "CLICK_COMMAND=${CMAKE_CURRENT_SOURCE_DIR}/mockclickcommand"
)
target_link_libraries(tst-systemupdate-click_manifest ${PLUGIN_LIBS})
