include_directories(${ANDR_PROP_INCLUDE_DIRS})
add_subdirectory(plugin)

pkg_search_module(MIR REQUIRED mirclient)
find_package(Qt5Gui)
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS} ${MIR_INCLUDE_DIRS})

add_library(uss-displays SHARED
    displays/helpers.h

    displays/display.h
    displays/displaymodel.h

    displays/mirdisplays.h
    displays/mirdisplays_impl.h

    displays/helpers.cpp
    displays/display.cpp
    displays/displaymodel.cpp
    displays/mirdisplays_impl.cpp
)
qt5_use_modules(uss-displays Quick)
set_target_properties(uss-displays PROPERTIES VERSION 0.0 SOVERSION 0.0)
target_link_libraries(uss-displays ${Qt5Gui_PRIVATE_LDFLAGS} ${MIR_LDFLAGS})

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
target_link_libraries(UbuntuBrightnessPanel SystemSettings ${ANDR_PROP_LDFLAGS} uss-displays)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Brightness)
install(TARGETS uss-displays DESTINATION ${PLUGIN_MODULE_DIR})
install(TARGETS UbuntuBrightnessPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)
install(FILES brightness.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-brightness.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/brightness)
