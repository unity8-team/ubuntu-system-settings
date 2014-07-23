set(DBUSMOCK_CMD "dbusmock")
include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/bluetooth)
add_definitions(-DTESTS)

add_executable(tst-bluetooth
    tst_bluetooth.cpp
    tst_devicemodel.cpp
    tst_device.cpp
    ../../../plugins/bluetooth/bluetooth.cpp
    ../../../plugins/bluetooth/devicemodel.cpp
    ../../../plugins/bluetooth/device.cpp
)

qt5_use_modules(tst-bluetooth Qml Quick Core DBus Test)
add_test(NAME tst-bluetooth COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tst-update-manager)

