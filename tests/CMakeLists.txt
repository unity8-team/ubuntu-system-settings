include_directories(
    ../src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${QTDBUSMOCK_INCLUDE_DIRS}
    ${QTDBUSTEST_INCLUDE_DIRS}
)

add_definitions(-DI18N_DOMAIN="ubuntu-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DPLUGIN_MODULE_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DSYSTEM_IMAGE_DBUS_TEMPLATE="${CMAKE_SOURCE_DIR}/tests/autopilot/ubuntu_system_settings/tests/systemimage.py")

add_library(test-plugin SHARED test-plugin.cpp test-plugin.h)
target_link_librarieS(test-plugin SystemSettings)
qt5_use_modules(test-plugin Core Qml)

add_library(test-plugin2 SHARED test-plugin2.cpp test-plugin2.h)
target_link_librarieS(test-plugin2 SystemSettings)
qt5_use_modules(test-plugin2 Core Qml)

add_executable(tst-plugins
    tst_plugins.cpp
    ../src/debug.cpp
    ../src/item-model.cpp
    ../src/plugin-manager.cpp
    ../src/plugin.cpp
    ../src/debug.h
    ../src/item-model.h
    ../src/plugin-manager.h
    ../src/plugin.h
)

add_executable(tst-arguments
    tst_arguments.cpp
    ../src/utils.cpp
)

add_executable(tst-systemimage
    tst_systemimage.cpp
    ../src/systemimage.cpp
    mocks/system-image-dbus/fakesystemimagedbus.cpp
)

qt5_use_modules(tst-plugins Core Qml Test)
target_link_libraries(tst-plugins SystemSettings ${GLIB_LDFLAGS})
add_test(tst-plugins tst-plugins)
set_tests_properties(tst-plugins PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=minimal")

qt5_use_modules(tst-arguments Core Test)
target_link_libraries(tst-arguments ${GLIB_LDFLAGS})
add_test(tst-arguments tst-arguments)

qt5_use_modules(tst-systemimage Core DBus Test)
target_link_libraries(tst-systemimage ${QTDBUSMOCK_LIBRARIES} ${QTDBUSTEST_LIBRARIES})
add_test(tst-systemimage tst-systemimage)

configure_file (test_code.py.in test_code.py)
configure_file (test_push_helper.py.in test_push_helper.py)
add_test(NAME python3 COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_code.py")
add_test(NAME test_push_helper COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_push_helper.py")

add_subdirectory(utils)

# QML tests that require graphical capabitlies.
add_custom_target(qmluitests)
add_dependencies(qmluitests UbuntuTest)

add_custom_target(qmltests)
add_dependencies(qmltests qmluitests)

add_subdirectory(mocks)
add_subdirectory(plugins)
