find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickTest REQUIRED)
add_definitions(-DTESTS)

set(TEST tst_notifications)
set(QML_SOURCES tst_PageComponent.qml)
set(SOURCES tst_QmlTests.cpp)

add_executable(${TEST} ${SOURCES})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(${TEST}
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickTest
)

add_test(${TEST} ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/${TEST}
    -input ${CMAKE_SOURCE_DIR}/tests/plugins/notifications/
    -import ${CMAKE_BINARY_DIR}/tests/mocks/
    -import ${CMAKE_SOURCE_DIR}/tests/plugins/notifications
    -import ${CMAKE_SOURCE_DIR}/src/
)
