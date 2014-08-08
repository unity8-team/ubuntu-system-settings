set(QML_SOURCES
    CallForwarding.qml
    CallWaiting.qml
    DualSim.qml
    Ofono.qml
    PageComponent.qml
    ServiceInfo.qml
    Services.qml
    SingleSim.qml
    dateUtils.js
)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(phone-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/phone)
install(FILES settings-phone.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES phone.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
