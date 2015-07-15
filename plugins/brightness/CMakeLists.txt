add_subdirectory(plugin)

set(QML_SOURCES
    PageComponent.qml
    PageResolution.qml
    PageRotation.qml
    displays.js
)

add_library(UbuntuBrightnessPanel MODULE
                brightness.h
                brightness.cpp
                display.h
                display.cpp
                displays.h
                displays.cpp
                plugin.h
                plugin.cpp
                ${QML_SOURCES})
qt5_use_modules(UbuntuBrightnessPanel Quick Qml DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Brightness)
install(TARGETS UbuntuBrightnessPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES brightness.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-brightness.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/brightness)
