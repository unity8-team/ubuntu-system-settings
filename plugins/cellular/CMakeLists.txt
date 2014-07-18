install(FILES cellular.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)

set(QML_SOURCES
    Apn.qml
    ChooseCarrier.qml
    CustomApn.qml
    Hotspot.qml
    HotspotSetup.qml
    PageComponent.qml
    rdosettings-helpers.js
)


add_library(UbuntuCellularPanel MODULE
  plugin.cpp
  plugin.h
  hotspotmanager.cpp
  hotspotmanager.h
  ofonoactivator.cpp
  ofonoactivator.h
  nm_manager_proxy.h
  nm_settings_proxy.h
  nm_settings_connection_proxy.h
  ${QML_SOURCES}
)
qt5_use_modules(UbuntuCellularPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Cellular)
install(TARGETS UbuntuCellularPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular)
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
