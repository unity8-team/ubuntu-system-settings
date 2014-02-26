set(QML_SOURCES PageComponent.qml Configuration.qml)

add_library(UbuntuUpdatePanel MODULE plugin.h system_update.h update_manager.h update.h
network/network.h download_tracker.h plugin.cpp system_update.cpp update_manager.cpp
update.cpp network/network.cpp download_tracker.cpp
${QML_SOURCES})

qt5_use_modules(UbuntuUpdatePanel Qml Quick DBus)
include_directories(/usr/include/apt-pkg/)

find_package (PkgConfig REQUIRED)
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})
target_link_libraries(UbuntuUpdatePanel
    apt-pkg
    ${UBUNTUONEAUTH_LDFLAGS}
    ${UBUNTU_DOWNLOAD_MANAGER_CLIENT_LDFLAGS}
    ${UBUNTU_DOWNLOAD_MANAGER_COMMON_LDFLAGS}
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Update)
install(TARGETS UbuntuUpdatePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/system-update)

install(FILES system-update.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES images/settings-system-update.svg images/distributor-logo.png DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)


set(QML_SOURCES_NOTIFICATION EntryComponent.qml)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(update-notification
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES_NOTIFICATION})


install(FILES update-notification.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES_NOTIFICATION} DESTINATION ${PLUGIN_QML_DIR}/update-notification)

add_subdirectory(plugin)
