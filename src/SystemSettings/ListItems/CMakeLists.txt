set(QML_SOURCES
    Standard.qml
)

set(PLUG_DIR ${PLUGIN_QML_DIR}/SystemSettings/ListItems)
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUG_DIR})
