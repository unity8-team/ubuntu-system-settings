include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/tests/mocks
    ${CMAKE_SOURCE_DIR}/plugins/system-update
    /usr/include/apt-pkg/
    ${QTDBUSMOCK_INCLUDE_DIRS}
    ${QTDBUSTEST_INCLUDE_DIRS}
)

pkg_check_modules(UAL REQUIRED ubuntu-app-launch-2)
add_definitions(${UAL_CFLAGS} ${UAL_CFLAGS_OTHER})
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

find_package(Qt5Test REQUIRED)
find_package(Qt5Sql REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PLUGIN_LIBS UpdatePlugin Qt5::Test Qt5::Sql ${UAL_LDFLAGS} ${UBUNTUONEAUTH_LDFLAGS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mockclickserver.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(tst-clickclient tst_clickclient.cpp)
add_test(tst-clickclient tst-clickclient)
target_link_libraries(tst-clickclient ${PLUGIN_LIBS})
set_tests_properties(
    tst-clickclient
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata;QT_QPA_PLATFORM=minimal"
)

add_executable(tst-tokendownloader tst_tokendownloader.cpp)
add_test(tst-tokendownloader tst-tokendownloader)
target_link_libraries(tst-tokendownloader ${PLUGIN_LIBS})
set_tests_properties(
    tst-tokendownloader
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009/metadata;"
)

add_executable(tst-clickmanager tst_clickmanager.cpp)
add_test(tst-clickmanager tst-clickmanager)
target_link_libraries(tst-clickmanager ${PLUGIN_LIBS})

add_executable(tst-imagemanager
    tst_imagemanager.cpp
    ${CMAKE_SOURCE_DIR}/src/systemimage.cpp
    ${CMAKE_SOURCE_DIR}/tests/mocks/system-image-dbus/fakesystemimagedbus.cpp
)
add_test(tst-imagemanager tst-imagemanager)
target_link_libraries(tst-imagemanager ${PLUGIN_LIBS} ${QTDBUSMOCK_LIBRARIES} ${QTDBUSTEST_LIBRARIES})

add_executable(tst-updatemanager tst_updatemanager.cpp)
add_test(tst-updatemanager tst-updatemanager)
target_link_libraries(tst-updatemanager ${PLUGIN_LIBS})

add_executable(tst-updatedb tst_updatedb.cpp)
add_test(tst-updatedb tst-updatedb)
target_link_libraries(tst-updatedb ${PLUGIN_LIBS})

add_executable(tst-update tst_update.cpp)
add_test(tst-update tst-update)
target_link_libraries(tst-update ${PLUGIN_LIBS})

add_executable(tst-updatemodel tst_updatemodel.cpp)
add_test(tst-updatemodel tst-updatemodel)
target_link_libraries(tst-updatemodel ${PLUGIN_LIBS})

add_executable(tst-updateplugin-helpers tst_helpers.cpp)
add_test(tst-updateplugin-helpers tst-updateplugin-helpers)
target_link_libraries(tst-updateplugin-helpers ${PLUGIN_LIBS})
set_tests_properties(
    tst-updateplugin-helpers
        PROPERTIES
        ENVIRONMENT "IGNORE_CREDENTIALS=1;URL_APPS=http://127.0.0.1:9009;FRAMEWORKS_FOLDER=${CMAKE_CURRENT_BINARY_DIR};CLICK_TOKEN_URL=http://example.org"
)

add_executable(tst-clickmanifest tst_clickmanifest.cpp)
add_test(tst-clickmanifest tst-clickmanifest)
target_link_libraries(tst-clickmanifest ${PLUGIN_LIBS})
set_tests_properties(
    tst-clickmanifest
        PROPERTIES
        ENVIRONMENT "CLICK_COMMAND=${CMAKE_CURRENT_SOURCE_DIR}/mockclickcommand"
)
