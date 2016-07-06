include(QmlTest)

set(qmltest_DEFAULT_TARGETS qmluitests)
set(qmltest_DEFAULT_PROPERTIES ENVIRONMENT "LC_ALL=C")
set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")

add_qml_test(mouse mouse
             IMPORT_PATHS
             ${CMAKE_SOURCE_DIR}/tests/plugins/mouse
)
