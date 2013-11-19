set(QML_SOURCES EntryComponent.qml)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(flight-mode-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})


install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/flight-mode)

install(FILES flight-mode.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-flight-mode.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
