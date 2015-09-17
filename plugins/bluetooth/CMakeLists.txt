include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(QML_SOURCES
ProvidePinCodeDialog.qml
ConfirmPasskeyDialog.qml
DisplayPasskeyDialog.qml
ProvidePasskeyDialog.qml
DevicePage.qml
PageComponent.qml
)

add_library(UbuntuBluetoothPanel MODULE
  adapter1.cpp
  properties.cpp
  objectmanager.cpp
  agent.cpp
  agentadaptor.cpp
  bluetooth.cpp
  device.cpp
  devicemodel.cpp
  plugin.cpp
  bluez_helper.h
  agent.h
  agentadaptor.h
  bluetooth.h
  device.h
  devicemodel.h
  plugin.h
  ${QML_SOURCES}
)

qt5_use_modules(UbuntuBluetoothPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Bluetooth)
install(TARGETS UbuntuBluetoothPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/bluetooth)

install(FILES bluetooth.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-bluetooth.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
