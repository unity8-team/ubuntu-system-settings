pkg_search_module(MIR_CORE REQUIRED mircore)
pkg_search_module(MIR_CLIENT REQUIRED mirclient)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/brightness
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness
    ${MIR_CORE_INCLUDE_DIRS}
    ${MIR_CLIENT_INCLUDE_DIRS}
)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Test REQUIRED)

set(PLUGIN_LIBS uss-displays Qt5::Core Qt5::Qml Qt5::Test Qt5::DBus)
set(FAKE_MIR_CODE
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness/fakemirclient.h
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness/fakeoutput.h
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness/fakeoutputmode.h
)

add_executable(tst-brightness
               ${FAKE_MIR_CODE}
               ${CMAKE_SOURCE_DIR}/plugins/brightness/brightness.cpp
               tst_brightness.cpp)
add_test(tst-brightness tst-brightness)
target_link_libraries(tst-brightness ${PLUGIN_LIBS} ${ANDR_PROP_LDFLAGS})

add_executable(tst-displayplugin-display tst_displayplugin_display.cpp)
add_test(tst-displayplugin-display tst-displayplugin-display)
target_link_libraries(tst-displayplugin-display ${PLUGIN_LIBS})

add_executable(tst-displayplugin-displaymodel tst_displayplugin_displaymodel.cpp)
add_test(tst-displayplugin-displaymodel tst-displayplugin-displaymodel)
target_link_libraries(tst-displayplugin-displaymodel ${PLUGIN_LIBS})
