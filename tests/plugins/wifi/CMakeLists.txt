include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/wifi
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/wifi
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTDBUSMOCK_INCLUDE_DIRS}
    ${QTDBUSTEST_INCLUDE_DIRS}
)

add_definitions(-DNETWORK_MANAGER_TEMPLATE="${CMAKE_SOURCE_DIR}/tests/autopilot/ubuntu_system_settings/tests/networkmanager.py")

add_executable(tst-wifidbushelper
    tst_wifidbushelper.cpp

    ${CMAKE_SOURCE_DIR}/plugins/wifi/nm_manager_proxy.h
    ${CMAKE_SOURCE_DIR}/plugins/wifi/nm_settings_proxy.h
    ${CMAKE_SOURCE_DIR}/plugins/wifi/nm_settings_connection_proxy.h

    ${CMAKE_SOURCE_DIR}/plugins/wifi/wifidbushelper.cpp
    ${CMAKE_SOURCE_DIR}/tests/mocks/plugins/wifi/fakenetworkmanager.cpp
)
qt5_use_modules(tst-wifidbushelper Core DBus Network Test)
target_link_libraries(tst-wifidbushelper ${QTDBUSMOCK_LIBRARIES} ${QTDBUSTEST_LIBRARIES})
add_test(tst-wifidbushelper tst-wifidbushelper)
