set(QML_SOURCES
    PageComponent.qml
)

install(FILES vpn.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES network-vpn.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/vpn)
