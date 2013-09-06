set(QML_SOURCES EntryComponent.qml)
install(FILES orientation-lock.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-orientation-lock.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/orientation-lock)
