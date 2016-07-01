include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/system-update
    /usr/include/apt-pkg/
)

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
find_package(Qt5Sql REQUIRED)
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})
set(PLUGIN_LIBS
    Qt5::Sql
    apt-pkg
    ${UBUNTUONEAUTH_LDFLAGS}
)

set(MOCK_SYSTEMUPDATE_SRCS
    plugin.cpp
    MockClickUpdateManager.cpp
    MockSystemImage.cpp
    MockSystemUpdate.cpp
    MockUpdate.cpp
    MockUpdateModel.cpp
    ${PLUGIN_SOURCES}
)

add_library(MockSystemUpdate MODULE ${MOCK_SYSTEMUPDATE_SRCS})
target_link_libraries(MockSystemUpdate ${PLUGIN_LIBS})
qt5_use_modules(MockSystemUpdate Qml Quick Core)

add_uss_mock(Ubuntu.SystemSettings.Update 1.0 Ubuntu/SystemSettings/Update
             TARGETS MockSystemUpdate)
