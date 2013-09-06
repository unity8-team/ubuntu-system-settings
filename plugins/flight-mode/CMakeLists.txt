set(QML_SOURCES EntryComponent.qml)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/flight-mode)

install(FILES flight-mode.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-flight-mode.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
