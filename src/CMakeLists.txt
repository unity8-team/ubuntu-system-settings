include_directories(${GLIB_INCLUDE_DIRS})

add_definitions(-DI18N_DIRECTORY="${CMAKE_INSTALL_PREFIX}/share/locale")
add_definitions(-DI18N_DOMAIN="ubuntu-system-settings")
add_definitions(-DPLUGIN_PRIVATE_MODULE_DIR="${PLUGIN_PRIVATE_MODULE_DIR}")
add_definitions(-DMANIFEST_DIR="${MANIFEST_DIR}")
add_definitions(-DPLUGIN_MANIFEST_DIR="${PLUGIN_MANIFEST_DIR}")
add_definitions(-DQML_DIR="${QML_DIR}")
add_definitions(-DPLUGIN_QML_DIR="${PLUGIN_QML_DIR}")
add_definitions(-DPLUGIN_MODULE_DIR="${PLUGIN_MODULE_DIR}")

add_subdirectory(SystemSettings)

set(USS_SOURCES
    debug.cpp
    i18n.cpp
    item-model.cpp
    main.cpp
    plugin-manager.cpp
    plugin.cpp
    systemimage.cpp
    utils.cpp
)

set(QML_SOURCES
    qml/CategorySection.qml
    qml/EntryComponent.qml
    qml/MainWindow.qml
    qml/UncategorizedItemsView.qml
    SystemSettings/ItemPage.qml
    SystemSettings/SettingsItemTitle.qml
)

QT5_ADD_RESOURCES(system-settings-resources ui.qrc)

add_executable(system-settings ${USS_SOURCES} ${QML_SOURCES} ${system-settings-resources})
qt5_use_modules(system-settings Core Gui Quick Qml DBus Widgets)
target_link_libraries(system-settings SystemSettings ${GLIB_LDFLAGS})
install(TARGETS system-settings RUNTIME DESTINATION bin)

add_library(uss-accountsservice SHARED accountsservice.h accountsservice.cpp)
qt5_use_modules(uss-accountsservice Core Qml DBus)
set_target_properties(uss-accountsservice PROPERTIES VERSION 0.0 SOVERSION 0.0)
install(TARGETS uss-accountsservice LIBRARY DESTINATION ${PLUGIN_MODULE_DIR} NAMELINK_SKIP)

add_library(uss-sessionservice SHARED sessionservice.h sessionservice.cpp)
qt5_use_modules(uss-sessionservice Core Qml DBus)
set_target_properties(uss-sessionservice PROPERTIES VERSION 0.0 SOVERSION 0.0)
install(TARGETS uss-sessionservice LIBRARY DESTINATION ${PLUGIN_MODULE_DIR} NAMELINK_SKIP)

add_library(uss-systemimage SHARED systemimage.h systemimage.cpp i18n.cpp)
qt5_use_modules(uss-systemimage Core Qml DBus)
set_target_properties(uss-systemimage PROPERTIES VERSION 0.0 SOVERSION 0.0)
install(TARGETS uss-systemimage LIBRARY DESTINATION ${PLUGIN_MODULE_DIR} NAMELINK_SKIP)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/url-map.ini DESTINATION ${PLUGIN_MANIFEST_DIR})
