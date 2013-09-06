set(QML_SOURCES EntryComponent.qml
    EraseEverything.qml
    PageComponent.qml
    ResetAllSettings.qml
    ResetLauncherHome.qml
)

install(FILES reset.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-reset.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/reset)
