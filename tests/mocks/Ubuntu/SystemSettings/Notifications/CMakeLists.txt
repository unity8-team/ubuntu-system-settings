include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/notifications/
    ${GIO_INCLUDE_DIRS}
    ${QTGSETTINGS_INCLUDE_DIRS}
)

set(MOCK_NOTIFICATIONS_SRCS
    ${CMAKE_SOURCE_DIR}/plugins/notifications/click_applications_model.cpp
    ${CMAKE_SOURCE_DIR}/plugins/notifications/click_applications_notify_model.cpp
    MockClickApplicationsModel.cpp
    MockClickApplicationsNotifyModel.cpp
    plugin.cpp
)

add_library(MockUbuntuNotificationsPanel MODULE ${MOCK_NOTIFICATIONS_SRCS})

target_link_libraries(MockUbuntuNotificationsPanel ${GIO_LDFLAGS} ${QTGSETTINGS_LDFLAGS})
qt5_use_modules(MockUbuntuNotificationsPanel Qml Quick Core)

add_uss_mock(Ubuntu.SystemSettings.Notifications 1.0 Ubuntu/SystemSettings/Notifications
             TARGETS MockUbuntuNotificationsPanel)
