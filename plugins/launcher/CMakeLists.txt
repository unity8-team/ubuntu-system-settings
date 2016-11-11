# include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(QML_SOURCES PageComponent.qml)

install(FILES launcher.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/launcher)
