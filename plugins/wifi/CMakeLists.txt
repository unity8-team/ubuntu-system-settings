add_library(UbuntuWifiPanel MODULE
  plugin.cpp
  unitymenumodelstack.cpp
  plugin.h
  unitymenumodelstack.h
)
qt5_use_modules(UbuntuWifiPanel Qml Quick DBus)

set(QML_SOURCES
AccessPoint.qml
BaseMenuItem.qml
DivMenuItem.qml
FramedMenuItem.qml
HLine.qml
IndicatorBase.qml
MenuItemFactory.qml
PageComponent.qml
RemoveBackground.qml
SectionMenuItem.qml
StandardMenuItem.qml
SwitchMenuItem.qml
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/wifi)
install(TARGETS UbuntuWifiPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES wifi.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-wifi.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/wifi)
