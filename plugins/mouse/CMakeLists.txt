add_subdirectory(plugin)

set(QML_SOURCES
    PageComponent.qml
    None.qml
    Connected.qml
    TapArea.qml
)

add_library(UbuntuMousePanel MODULE
                mouse.h
                mouse.cpp
                touchpad.h
                touchpad.cpp
                plugin.h
                plugin.cpp
                ${QML_SOURCES})
qt5_use_modules(UbuntuMousePanel Quick Qml DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Mouse)
install(TARGETS UbuntuMousePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES mouse.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-mouse.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/mouse)
