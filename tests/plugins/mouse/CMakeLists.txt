find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickTest REQUIRED)
add_definitions(-DTESTS)

set(XVFB_CMD
    LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1
    xvfb-run -a -s "-screen 0 640x480x24"
)

set(QML_SOURCES tst_mouse.qml)

add_executable(tst-mouse tst_mouse.cpp)

target_link_libraries(tst-mouse
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickTest
)

add_test(NAME tst-mouse COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-mouse
    -input ${CMAKE_SOURCE_DIR}/tests/plugins/mouse/
    -import ${PLUGIN_QML_DIR}
    -import ${PLUGIN_PRIVATE_MODULE_DIR}
    -import ${CMAKE_SOURCE_DIR}/tests/plugins/mouse
)
