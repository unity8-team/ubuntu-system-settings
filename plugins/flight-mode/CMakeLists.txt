set(QML_SOURCES EntryComponent.qml)

add_library(FlightModeHelper MODULE
  plugin.cpp
  plugin.h
  ${QML_SOURCES}
)
qt5_use_modules(FlightModeHelper Qml Quick DBus)
set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/flight-mode)

install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/flight-mode)

install(TARGETS FlightModeHelper DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})

install(FILES flight-mode.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-flight-mode.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
