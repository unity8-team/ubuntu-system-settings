if(ANDR_PROP_FOUND)
    add_definitions(-DANDROID_PROPERTIES)
endif()

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
)

include_directories(${ANDR_PROP_INCLUDE_DIRS})

add_library(UbuntuWifiPanel MODULE
  certhandler.cpp
  plugin.cpp
  previousnetworkmodel.cpp
  unitymenumodelstack.cpp
  wifi.cpp
  wifidbushelper.cpp
  certhandler.h
  nm_manager_proxy.h
  nm_settings_proxy.h
  nm_settings_connection_proxy.h
  plugin.h
  previousnetworkmodel.h
  unitymenumodelstack.h
  wifidbushelper.h
  ${QML_SOURCES}
)
target_link_libraries(UbuntuWifiPanel ${ANDR_PROP_LDFLAGS})
qt5_use_modules(UbuntuWifiPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Wifi)
install(TARGETS UbuntuWifiPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES wifi.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-wifi.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/wifi)
