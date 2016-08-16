# add_qml_test macro
include(QmlTest)

add_subdirectory(security-privacy)
add_subdirectory(system-update)
add_subdirectory(bluetooth)
add_subdirectory(wifi)

set(qmltest_DEFAULT_TARGETS qmluitests)
set(qmltest_DEFAULT_PROPERTIES ENVIRONMENT "LC_ALL=C")

set(qmltest_DEFAULT_IMPORT_PATHS
  ${CMAKE_BINARY_DIR}/tests/utils/modules
)

add_qml_test(mouse mouse
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/mouse
             ${qmltest_DEFAULT_IMPORT_PATHS})

add_qml_test(wifi OtherNetwork
             IMPORT_PATHS
             ${CMAKE_BINARY_DIR}/tests/mocks/
             ${CMAKE_SOURCE_DIR}/src
             ${CMAKE_CURRENT_SOURCE_DIR}/wifi
             ${qmltest_DEFAULT_IMPORT_PATHS})

add_subdirectory(notifications)
