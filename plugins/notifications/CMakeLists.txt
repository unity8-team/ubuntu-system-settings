set(QML_SOURCES PageComponent.qml)
SET (CMAKE_AUTOMOC ON)

add_library(UbuntuNotificationsPanel MODULE plugin.h notification_manager.h
notification_item.h notification_item.cpp
plugin.cpp notification_manager.cpp
${QML_SOURCES})

include_directories(${GIO_INCLUDE_DIRS})
target_link_libraries(UbuntuNotificationsPanel ${GLIB_LDFLAGS} ${GIO_LDFLAGS})
qt5_use_modules(UbuntuNotificationsPanel Qml Quick)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Notifications)
install(TARGETS UbuntuNotificationsPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/notifications)

install(FILES notifications.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
