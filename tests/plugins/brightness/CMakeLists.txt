include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/brightness
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness
    ${QTDBUSMOCK_INCLUDE_DIRS}
    # ${QTDBUSTEST_INCLUDE_DIRS}
    ${MIR_INCLUDE_DIRS}
    ${ANDR_PROP_INCLUDE_DIRS}
)

pkg_search_module(MIR REQUIRED mirclient)
find_package(Qt5Test REQUIRED)
find_package(Qt5DBus REQUIRED)

#set(CMAKE_AUTOMOC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PLUGIN_LIBS uss-displays Qt5::Test Qt5::DBus)

add_executable(tst-brightness
               ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/brightness/fakemirclient.h
               tst_brightness.cpp
               ${CMAKE_SOURCE_DIR}/plugins/brightness/brightness.cpp)
add_test(tst-brightness tst-brightness)
target_link_libraries(tst-brightness ${PLUGIN_LIBS} ${ANDR_PROP_LDFLAGS} ${QTDBUSMOCK_LIBRARIES} ${QTDBUSTEST_LIBRARIES})
