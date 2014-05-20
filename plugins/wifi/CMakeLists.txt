set(QML_SOURCES
AccessPoint.qml
BaseMenuItem.qml
DivMenuItem.qml
FramedMenuItem.qml
HLine.qml
IndicatorBase.qml
MenuItemFactory.qml
OtherNetwork.qml
PageComponent.qml
RemoveBackground.qml
SectionMenuItem.qml
StandardMenuItem.qml
SwitchMenuItem.qml
)

add_library(UbuntuWifiPanel MODULE
  wifidbushelper.cpp
  plugin.cpp
  unitymenumodelstack.cpp
  wifidbushelper.h
  plugin.h
  unitymenumodelstack.h
  ${QML_SOURCES}
)
qt5_use_modules(UbuntuWifiPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Wifi)
install(TARGETS UbuntuWifiPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES wifi.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-wifi.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/wifi)
