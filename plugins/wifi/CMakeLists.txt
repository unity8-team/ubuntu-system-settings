set(QML_SOURCES
AccessPoint.qml
BaseMenuItem.qml
CertPicker.qml
CertDialog.qml
Common.qml
DivMenuItem.qml
FramedMenuItem.qml
HLine.qml
IndicatorBase.qml
MenuItemFactory.qml
NetworkDetails.qml
NetworkDetailsBrief.qml
OtherNetwork.qml
PageComponent.qml
PreviousNetworks.qml
RemoveBackground.qml
SectionMenuItem.qml
StandardMenuItem.qml
SwitchMenuItem.qml
WifiDisplays.qml
)

add_library(UbuntuWifiPanel MODULE
  aethercast_device.cpp
  aethercast_manager.cpp
  certhandler.cpp
  device.cpp
  devicemodel.cpp
  displays.cpp
  freedesktop_properties.cpp
  freedesktop_objectmanager.cpp
  plugin.cpp
  previousnetworkmodel.cpp
  unitymenumodelstack.cpp
  wifidbushelper.cpp
  aethercast_helper.h
  aethercast_device.h
  aethercast_manager.h
  certhandler.h
  device.h
  devicemodel.h
  displays.h
  freedesktop_properties.h
  freedesktop_objectmanager.h
  nm_manager_proxy.h
  nm_settings_proxy.h
  nm_settings_connection_proxy.h
  plugin.h
  previousnetworkmodel.h
  unitymenumodelstack.h
  wifidbushelper.h
  ${QML_SOURCES}
)
qt5_use_modules(UbuntuWifiPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Wifi)
install(TARGETS UbuntuWifiPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES wifi.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-wifi.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/wifi)
