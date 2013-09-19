add_library(UbuntuResetPanel MODULE
  plugin.cpp
  reset.cpp
  plugin.h
  reset.h
)
qt5_use_modules(UbuntuResetPanel Qml Quick DBus)

set(QML_SOURCES EntryComponent.qml
    EraseEverything.qml
    PageComponent.qml
    ResetAllSettings.qml
    ResetLauncherHome.qml
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/reset)
install(TARGETS UbuntuResetPanel DESTINATION ${PLUG_DIR})
install(FILES reset.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-reset.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/reset)
