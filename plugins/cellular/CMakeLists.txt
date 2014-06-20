set(QML_SOURCES
    ChooseCarrier.qml
    PageComponent.qml
    utilities.js
)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(cellular-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Cellular)
install(FILES cellular.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular)
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
