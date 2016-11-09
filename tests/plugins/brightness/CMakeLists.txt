include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/brightness
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness
    ${MIR_INCLUDE_DIRS}
    ${ANDR_PROP_INCLUDE_DIRS}
)

pkg_search_module(MIR REQUIRED mirclient)
find_package(Qt5Test REQUIRED)
find_package(Qt5DBus REQUIRED)

set(PLUGIN_LIBS uss-displays Qt5::Test Qt5::DBus)

add_executable(tst-brightness
               ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness/fakemirclient.h
               tst_brightness.cpp
               ${CMAKE_SOURCE_DIR}/plugins/brightness/brightness.cpp)
add_test(tst-brightness tst-brightness)
target_link_libraries(tst-brightness ${PLUGIN_LIBS} ${ANDR_PROP_LDFLAGS})

add_executable(tst-displayplugin-display
               tst_displayplugin_display.cpp
               ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/display.cpp)
add_test(tst-displayplugin-display tst-displayplugin-display)
target_link_libraries(tst-displayplugin-display ${PLUGIN_LIBS})

add_executable(tst-displayplugin-displaymodel
               tst_displayplugin_displaymodel.cpp
               ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/display.cpp
               ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/displaymodel.cpp)
add_test(tst-displayplugin-displaymodel tst-displayplugin-displaymodel)
target_link_libraries(tst-displayplugin-displaymodel ${PLUGIN_LIBS})
