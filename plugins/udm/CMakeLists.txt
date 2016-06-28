include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(QML_SOURCES PageComponent.qml)


# This plugin is only an example so let's not install it.
install(FILES udm.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/udm)
