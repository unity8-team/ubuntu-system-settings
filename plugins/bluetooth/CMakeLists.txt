include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(UbuntuBluetoothPanel MODULE
  agent.cpp
  agentadaptor.cpp
  bluetooth.cpp
  device.cpp
  devicemodel.cpp
  plugin.cpp
  killswitch.cpp
  agent.h 
  agentadaptor.h
  bluetooth.h
  device.h
  devicemodel.h
  killswitch.h
  plugin.h
)

qt5_use_modules(UbuntuBluetoothPanel Qml Quick DBus)

set(QML_SOURCES
ProvidePinCodeDialgo.qml
ConfirmPasskeyDialog.qml
ProvidePasskeyDialog.qml
PageComponent.qml 
)


set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Bluetooth)
install(TARGETS UbuntuBluetoothPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/bluetooth)

install(FILES bluetooth.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-bluetooth.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
