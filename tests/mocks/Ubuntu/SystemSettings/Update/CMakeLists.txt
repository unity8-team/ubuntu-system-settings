include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/plugins/system-update
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/system-update
    /usr/include/apt-pkg/
)

pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})
set(PLUGIN_LIBS
    UpdatePlugin
    apt-pkg
    ${UBUNTUONEAUTH_LDFLAGS}
)

set(MOCK_SYSTEMUPDATE_SRCS
    plugin.cpp
    MockSystemImage.cpp
    MockUpdateManager.cpp
    MockUpdateModel.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    ${PLUGIN_SOURCES}
)

add_library(MockSystemUpdate MODULE ${MOCK_SYSTEMUPDATE_SRCS})
target_link_libraries(MockSystemUpdate ${PLUGIN_LIBS})
qt5_use_modules(MockSystemUpdate Qml Quick Core Sql)

add_uss_mock(Ubuntu.SystemSettings.Update 1.0 Ubuntu/SystemSettings/Update
             TARGETS MockSystemUpdate)
