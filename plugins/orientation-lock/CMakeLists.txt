set(QML_SOURCES EntryComponent.qml)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(orientation-lock-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})


install(FILES orientation-lock.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-orientation-lock.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/orientation-lock)
