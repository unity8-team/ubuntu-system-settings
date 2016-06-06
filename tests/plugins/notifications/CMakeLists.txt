find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickTest REQUIRED)
add_definitions(-DTESTS)

set(XVFB_COMMAND)
find_program(XVFBRUN xvfb-run)
if(XVFBRUN)
  set(XVFB_COMMAND ${XVFBRUN} -s "-screen 0 640x480x24" -a)
else()
  message(WARNING "Cannot find xvfb-run.")
endif()

set(TEST tst_notifications)
set(QML_SOURCES
    tst_PageComponent.qml
)
set(SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/notifications/click_applications_model.cpp
    ${CMAKE_SOURCE_DIR}/plugins/notifications/click_applications_notify_model.cpp
    tst_QmlTests.cpp
)

add_executable(${TEST} ${SOURCES})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/plugins/notifications/
    ${GIO_INCLUDE_DIRS}
    ${QTGSETTINGS_INCLUDE_DIRS}
)

target_link_libraries(${TEST}
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickTest
    ${GIO_LDFLAGS}
    ${QTGSETTINGS_LDFLAGS}
)

add_test(${TEST} ${XVFB_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/${TEST}
    -input ${CMAKE_SOURCE_DIR}/tests/plugins/notifications/
    -import ${CMAKE_SOURCE_DIR}/src/
    -import ${CMAKE_SOURCE_DIR}/tests/plugins/notifications
)
