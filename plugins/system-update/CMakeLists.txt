SET (CMAKE_AUTOMOC ON)
set(QML_SOURCES
    ClickUpdate.qml
    Configuration.qml
    DownloadHandler.qml
    Global.qml
    ImageUpdate.qml
    ImageUpdatePrompt.qml
    NotAuthenticatedNotification.qml
    PageComponent.qml
    Updates.qml
    Update.qml
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    /usr/include/apt-pkg/
)

pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

add_library(UbuntuUpdatePanel MODULE
            # updatestore.cpp
            clickapiclient.cpp
            clicktokendownloader.cpp
            clickupdatemanager.cpp
            helpers.cpp
            plugin.cpp
            systemupdate.cpp
            update.cpp
            updatedb.cpp
            updatestruct.cpp
            updatemodel.cpp
            ${QML_SOURCES})

qt5_use_modules(UbuntuUpdatePanel Core Qml Quick Network DBus Sql)

target_link_libraries(UbuntuUpdatePanel
    uss-systemimage
    apt-pkg
    ${UBUNTUONEAUTH_LDFLAGS}
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Update)
install(TARGETS UbuntuUpdatePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/system-update)

install(FILES system-update.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES images/settings-system-update.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)

set(QML_SOURCES_NOTIFICATION EntryComponent.qml)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(update-notification
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES_NOTIFICATION})

install(FILES update-notification.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES_NOTIFICATION} DESTINATION ${PLUGIN_QML_DIR}/update-notification)

# add_subdirectory(plugin)
