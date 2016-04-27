include_directories(${ANDR_PROP_INCLUDE_DIRS})
add_subdirectory(plugin)

set(QML_SOURCES
    PageComponent.qml
    WifiDisplays.qml
    WifiDisplaysAlert.qml
)

add_library(UbuntuBrightnessPanel MODULE
                aethercast/aethercast_helper.h
                aethercast/aethercast_device.h
                aethercast/aethercast_manager.h
                aethercast/device.h
                aethercast/devicemodel.h
                aethercast/displays.h
                aethercast/freedesktop_properties.h
                aethercast/freedesktop_objectmanager.h
                plugin.h
                brightness.h
                aethercast/aethercast_device.cpp
	        aethercast/aethercast_manager.cpp
	        aethercast/device.cpp
	        aethercast/devicemodel.cpp
                aethercast/displays.cpp
	        aethercast/freedesktop_properties.cpp
	        aethercast/freedesktop_objectmanager.cpp
                plugin.cpp
                brightness.cpp
                ${QML_SOURCES})
qt5_use_modules(UbuntuBrightnessPanel Quick Qml DBus)
target_link_libraries(UbuntuBrightnessPanel SystemSettings ${ANDR_PROP_LDFLAGS})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Brightness)
install(TARGETS UbuntuBrightnessPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES brightness.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-brightness.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/brightness)


