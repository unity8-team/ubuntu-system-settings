# add_qml_test macro
include(QmlTest)

add_subdirectory(security-privacy)
add_subdirectory(system-update)
add_subdirectory(bluetooth)
add_subdirectory(notifications)

set(qmltest_DEFAULT_TARGETS qmluitests)
set(qmltest_DEFAULT_PROPERTIES ENVIRONMENT "LC_ALL=C")
set(qmltest_DEFAULT_IMPORT_PATHS
  ${CMAKE_BINARY_DIR}/tests/utils/modules
)

add_qml_test(mouse mouse
    IMPORT_PATHS
    ${CMAKE_SOURCE_DIR}/tests/plugins/mouse
)

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

# add_qml_test(system-update systemupdate_pagecomponent
#     IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
# )

add_qml_test(system-update systemupdate_entrycomponent
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_noauthentication
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)

add_qml_test(system-update systemupdate_download_handler
    IMPORT_PATHS ${SYSTEMUPDATE_IMPORT_PATHS}
)
