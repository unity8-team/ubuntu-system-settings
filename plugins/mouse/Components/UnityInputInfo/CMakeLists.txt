set(QML_SOURCES
    UnityInputInfo.qml
    qmldir
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/mouse/Components/UnityInputInfo/)

# add a phony target to get the files visible in Qt Creator.
add_custom_target(
    plugins_mouse_components_sources
    SOURCES ${QML_SOURCES}
)
