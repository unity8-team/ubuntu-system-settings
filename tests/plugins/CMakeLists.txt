# add_qml_test macro
include(QmlTest)

add_subdirectory(security-privacy)
add_subdirectory(system-update)
add_subdirectory(bluetooth)
add_subdirectory(wifi)
add_subdirectory(notifications)

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

set(SYSTEMUPDATE_IMPORT_PATHS
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/tests/utils/modules
    ${CMAKE_BINARY_DIR}/tests/mocks/
    ${CMAKE_SOURCE_DIR}/tests/plugins/system-update
)

add_qml_test(system-update systemupdate_update
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_global
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_pagecomponent
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_entrycomponent
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_noauthentication
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_download_handler
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(main MainWindow
    IMPORT_PATHS
    ${CMAKE_BINARY_DIR}/tests/mocks/
    ${CMAKE_SOURCE_DIR}/tests/plugins/main
    ${qmltest_DEFAULT_IMPORT_PATHS}
)

set(LAUNCHER_IMPORT_PATHS
    ${CMAKE_BINARY_DIR}/tests/mocks/
    ${CMAKE_SOURCE_DIR}/tests/plugins/launcher
    ${qmltest_DEFAULT_IMPORT_PATHS}
)
add_qml_test(launcher LauncherPageComponent IMPORT_PATHS ${LAUNCHER_IMPORT_PATHS})
