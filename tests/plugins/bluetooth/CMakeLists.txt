set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")
include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/bluetooth)
include_directories(${QTDBUSMOCK_INCLUDE_DIRS})
include_directories(${QTDBUSTEST_INCLUDE_DIRS})
add_definitions(-DTESTS)

add_executable(tst-bluetooth
    tst_bluetooth.cpp
    fakebluez.cpp
    ../../../plugins/bluetooth/bluetooth.cpp
    ../../../plugins/bluetooth/devicemodel.cpp
    ../../../plugins/bluetooth/device.cpp
    ../../../plugins/bluetooth/agent.cpp
    ../../../plugins/bluetooth/agentadaptor.cpp
)

target_link_libraries(tst-bluetooth
    ${QTDBUSMOCK_LIBRARIES}
    ${QTDBUSTEST_LIBRARIES}
)

add_executable(tst-bluetooth-devicemodel
    tst_devicemodel.cpp
    fakebluez.cpp
    ../../../plugins/bluetooth/bluetooth.cpp
    ../../../plugins/bluetooth/devicemodel.cpp
    ../../../plugins/bluetooth/device.cpp
    ../../../plugins/bluetooth/agent.cpp
    ../../../plugins/bluetooth/agentadaptor.cpp
)

target_link_libraries(tst-bluetooth-devicemodel
    ${QTDBUSMOCK_LIBRARIES}
    ${QTDBUSTEST_LIBRARIES}
)

add_executable(tst-bluetooth-device
    tst_devicemodel.cpp
    fakebluez.cpp
    ../../../plugins/bluetooth/bluetooth.cpp
    ../../../plugins/bluetooth/devicemodel.cpp
    ../../../plugins/bluetooth/device.cpp
    ../../../plugins/bluetooth/agent.cpp
    ../../../plugins/bluetooth/agentadaptor.cpp
)

target_link_libraries(tst-bluetooth-device
    ${QTDBUSMOCK_LIBRARIES}
    ${QTDBUSTEST_LIBRARIES}
)

qt5_use_modules(tst-bluetooth Qml Quick Core DBus Test)
qt5_use_modules(tst-bluetooth-devicemodel Qml Quick Core DBus Test)
qt5_use_modules(tst-bluetooth-device Qml Quick Core DBus Test)
#add_test(NAME tst-bluetooth
#         COMMAND ${XVFB_CMD} dbus-test-runner -t ${CMAKE_CURRENT_BINARY_DIR}/tst-bluetooth)
#add_test(NAME tst-bluetooth-devicemodel
#         COMMAND ${XVFB_CMD} dbus-test-runner -t ${CMAKE_CURRENT_BINARY_DIR}/tst-bluetooth-devicemodel)
#add_test(NAME tst-bluetooth-device
#         COMMAND ${XVFB_CMD} dbus-test-runner -t ${CMAKE_CURRENT_BINARY_DIR}/tst-bluetooth-device)

add_test(NAME tst-bluetooth
         COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-bluetooth)
add_test(NAME tst-bluetooth-devicemodel
         COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-bluetooth-devicemodel)
add_test(NAME tst-bluetooth-device
         COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-bluetooth-device)
