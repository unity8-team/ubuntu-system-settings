add_subdirectory(plugin)

set(QML_SOURCES
    Common.qml
    HotspotSetup.qml
    PageComponent.qml
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Hotspot)

install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES hotspot.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-hotspot.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/hotspot)
