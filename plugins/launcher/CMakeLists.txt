add_subdirectory(plugin)

set(QML_SOURCES PageComponent.qml)
set(PANEL_SOURCES
    launcher.h
    launcher_impl.cpp
    plugin.cpp
    plugin.h
    ${QML_SOURCES}
)

add_library(UbuntuLauncherPanel MODULE ${PANEL_SOURCES} ${QML_SOURCES})
qt5_use_modules(UbuntuLauncherPanel Qml Widgets)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Launcher)
install(TARGETS UbuntuLauncherPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)

install(FILES launcher.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/launcher)
