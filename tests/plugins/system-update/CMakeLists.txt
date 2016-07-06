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

pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PLUGIN_HEADERS
    ${CMAKE_SOURCE_DIR}/plugins/system-update/network/manager.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/client.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/manifest.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/sso.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/click/tokendownloader_factory.h
)

set(PLUGIN_SOURCES
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
)

set(PLUGIN_LIBS
    Qt5::Test
    Qt5::Sql
    Qt5::Network
    apt-pkg
    ${UBUNTUONEAUTH_LDFLAGS}
)

# Install a mock click server
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/mockclickserver.py
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})

# Install a mock click command
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/mockclickcommand
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})

add_executable(tst-click_client
    tst_click_client.cpp
    mockclickserver.h

    ${PLUGIN_HEADERS}
    ${PLUGIN_SOURCES}
)
add_test(tst-click_client tst-click_client)
set_tests_properties(
    tst-click_client
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata"
)
target_link_libraries(tst-click_client ${PLUGIN_LIBS})

# add_executable(tst-clicktokendownloader
#                mocks/mocknetworkaccessmanager.cpp
#                tst_clicktokendownloader.cpp
#                ${PLUGIN_SOURCES})
# add_test(tst-clicktokendownloader tst-clicktokendownloader)
# set_tests_properties(
#     tst-clicktokendownloader
#         PROPERTIES
#         ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata"
# )
# target_link_libraries(tst-clicktokendownloader ${PLUGIN_LIBS})

add_executable(tst-clickupdatemanager
    tst_clickupdatemanager.cpp
    ${PLUGIN_HEADERS}
    ${PLUGIN_SOURCES}
)
add_test(tst-clickupdatemanager tst-clickupdatemanager)
set_tests_properties(
    tst-clickupdatemanager
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1"
)

target_link_libraries(tst-clickupdatemanager ${PLUGIN_LIBS})

# add_executable(tst-updatedb
#                tst_updatedb.cpp
#                ${PLUGIN_SOURCES})
# add_test(tst-updatedb tst-updatedb)
# target_link_libraries(tst-updatedb ${PLUGIN_LIBS})

# add_executable(tst-update
#                tst_update.cpp
#                ${PLUGIN_SOURCES})
# add_test(tst-update tst-update)
# target_link_libraries(tst-update ${PLUGIN_LIBS})

# add_executable(tst-updatemodel
#                tst_updatemodel.cpp
#                ${PLUGIN_SOURCES})
# add_test(tst-updatemodel tst-updatemodel)
# target_link_libraries(tst-updatemodel ${PLUGIN_LIBS})

# add_executable(tst-updatepluginhelpers
#                tst_helpers.cpp
#                ${PLUGIN_SOURCES})
# add_test(tst-updatepluginhelpers tst-updatepluginhelpers)
# set_tests_properties(
#     tst-updatepluginhelpers
#         PROPERTIES
#         ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009;FRAMEWORKS_FOLDER=${CMAKE_CURRENT_BINARY_DIR};CLICK_TOKEN_URL=http://example.org;"
# )
# target_link_libraries(tst-updatepluginhelpers ${PLUGIN_LIBS})

add_subdirectory(qmltests)
