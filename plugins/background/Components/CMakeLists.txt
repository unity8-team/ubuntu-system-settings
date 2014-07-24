set(QML_SOURCES
    AddRemove.qml
    selected-overlay.png
    ListItemsHeader.qml
    HighlightedOverlay.qml
    SelectedOverlay.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/background/Components)
