# add_qml_test macro
include(QmlTest)

add_subdirectory(security-privacy)
add_subdirectory(system-update)
add_subdirectory(bluetooth)

set(qmltest_DEFAULT_TARGETS qmluitests)
set(qmltest_DEFAULT_PROPERTIES ENVIRONMENT "LC_ALL=C")

set(COMMON_IMPORT_PATHS
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/tests/utils/modules
  ${CMAKE_BINARY_DIR}/tests/mocks/
)

add_qml_test(mouse mouse
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/mouse
)

add_qml_test(system-update update
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)

add_qml_test(system-update click_update
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)

add_qml_test(system-update image_update
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)

add_qml_test(system-update global
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)

add_qml_test(system-update updates
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)

add_qml_test(system-update noauthentication
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)

add_qml_test(system-update systemimage_handler
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
             ${COMMON_IMPORT_PATHS}
)
