set(QML_SOURCES
    Icon.qml
    IconProgression.qml
    SingleControl.qml
    SingleValue.qml
    SingleValueProgression.qml
    Standard.qml
    StandardProgression.qml
)

set(PLUG_DIR ${PLUGIN_QML_DIR}/SystemSettings/ListItems)
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUG_DIR})
