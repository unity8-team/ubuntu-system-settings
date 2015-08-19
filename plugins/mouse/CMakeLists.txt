add_subdirectory(plugin)
add_subdirectory(InputInfo)
add_subdirectory(Components)

set(QML_SOURCES
    PageComponent.qml
    None.qml
    Connected.qml
    TapArea.qml
    PrimaryButtonSelector.qml
)

install(FILES mouse.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-mouse.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/mouse)
