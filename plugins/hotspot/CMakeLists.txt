add_subdirectory(plugin)

set(QML_SOURCES
    Common.qml
    HotspotSetup.qml
    PageComponent.qml
)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(hotspot-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Hotspot)

install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES hotspot.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-hotspot.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/hotspot)
