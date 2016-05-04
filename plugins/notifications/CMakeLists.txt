set(QML_SOURCES PageComponent.qml)
SET (CMAKE_AUTOMOC ON)

set(ubuntu_notifications_panel_HDRS
    plugin.h
    click_applications_model.h
    )

set(ubuntu_notifications_panel_SRCS
    plugin.cpp
    click_applications_model.cpp
    )

add_library(UbuntuNotificationsPanel MODULE ${ubuntu_notifications_panel_HDRS} ${ubuntu_notifications_panel_SRCS} ${QML_SOURCES})

add_definitions(-DQT_NO_KEYWORDS)
include_directories(${CLICK_INCLUDE_DIRS})

target_link_libraries(UbuntuNotificationsPanel ${CLICK_LDFLAGS})
qt5_use_modules(UbuntuNotificationsPanel Qml Quick)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Notifications)
install(TARGETS UbuntuNotificationsPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/notifications)

install(FILES notifications.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
