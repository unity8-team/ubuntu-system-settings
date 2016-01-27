add_subdirectory(InputInfo)
add_subdirectory(Components)

set(QML_SOURCES
    PageComponent.qml
    None.qml
    Connected.qml
    ItemTitle.qml
    TapArea.qml
    PrimaryButtonSelector.qml
    SectionHeader.qml
)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(mouse-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

#install(FILES mouse.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-mouse.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/mouse)
