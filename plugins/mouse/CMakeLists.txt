set(QML_SOURCES PageComponent.qml TapArea.qml None.qml Connected.qml)

add_custom_target(mouse SOURCES ${QML_SOURCES})


install(FILES mouse.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-mouse.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/mouse)
