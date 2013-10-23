install(FILES cellular.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)

set(QML_SOURCES
    ChooseCarrier.qml
    PageComponent.qml
)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(cellular-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular)
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
