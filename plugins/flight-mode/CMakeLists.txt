set(QML_SOURCES EntryComponent.qml)

add_library(FlightModeHelper MODULE
  flight-mode-helper.cpp
  flight-mode-helper.h
  plugin.cpp
  plugin.h
  # Proxy class for interface org.freedesktop.URfkill, /org/freedesktop/URfkill
  # manually generated with qdbusxml2cpp
  # if the API changes, get the introspection data
  # from the service and regenerate the file
  urfkill-proxy.h
  ${QML_SOURCES}
)
qt5_use_modules(FlightModeHelper Qml Quick DBus)
set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/FlightMode)

install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/flight-mode)

install(TARGETS FlightModeHelper DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})

install(FILES flight-mode.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-flight-mode.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
