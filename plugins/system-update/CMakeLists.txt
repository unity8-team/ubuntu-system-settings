set(CMAKE_AUTOMOC ON)
set(QML_SOURCES
    ClickUpdateDelegate.qml
    Configuration.qml
    ChangelogExpander.qml
    DownloadHandler.qml
    Global.qml
    ImageUpdatePrompt.qml
    NotAuthenticatedNotification.qml
    PageComponent.qml
    UpdateDelegate.qml
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/include/apt-pkg/
)

pkg_check_modules(UAL REQUIRED ubuntu-app-launch-2)
add_definitions(${UAL_CFLAGS} ${UAL_CFLAGS_OTHER})
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

add_library(UpdatePlugin SHARED
    click/apiclient.h
    click/manager.h
    click/manifest.h
    click/sessiontoken.h
    click/sso.h
    click/tokendownloader.h
    click/tokendownloader_factory.h

    click/apiclient_impl.cpp
    click/manifest_impl.cpp
    click/sessiontoken_impl.cpp
    click/sso_impl.cpp
    click/tokendownloader_impl.cpp
    click/tokendownloader_factory_impl.cpp
    click/manager_impl.cpp

    image/imagemanager.h
    image/imagemanager_impl.cpp

    network/accessmanager.h
    network/accessmanager_impl.cpp

    helpers.cpp
    update.cpp
    updatedb.cpp
    updatemodel.cpp
    updatemanager.cpp
    ../../src/i18n.cpp
)
qt5_use_modules(UpdatePlugin Quick Core Network Sql)
target_link_libraries(UpdatePlugin
    apt-pkg
    uss-systemimage
    ${UAL_LDFLAGS}
    ${UBUNTUONEAUTH_LDFLAGS}
)

add_library(UbuntuUpdatePanel MODULE
    plugin.cpp
    ${QML_SOURCES}
)
qt5_use_modules(UbuntuUpdatePanel Qml Quick)
target_link_libraries(UbuntuUpdatePanel UpdatePlugin)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Update)
install(TARGETS UpdatePlugin DESTINATION ${PLUGIN_MODULE_DIR})
install(TARGETS UbuntuUpdatePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/system-update)

install(FILES system-update.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-system-update.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)

set(QML_SOURCES_NOTIFICATION EntryComponent.qml)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(update-notification
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES_NOTIFICATION})

install(FILES update-notification.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES_NOTIFICATION} DESTINATION ${PLUGIN_QML_DIR}/update-notification)
