add_subdirectory(plugin)

pkg_search_module(MIR REQUIRED mirclient)
find_package(Qt5Gui)
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS} ${MIR_INCLUDE_DIRS})

set(QML_SOURCES
    PageComponent.qml
    PageResolution.qml
    PageRotation.qml
    ExternalDisplay.qml
)

add_library(uss-displays STATIC
                display.h
                display.cpp
                displaymodel.h
                displaymodel.cpp
                displays.h
                displays.cpp)
qt5_use_modules(uss-displays Quick DBus)
set_target_properties(uss-displays PROPERTIES VERSION 0.0 SOVERSION 0.0)
target_link_libraries(uss-displays ${Qt5Gui_PRIVATE_LDFLAGS} ${MIR_LDFLAGS})

add_library(UbuntuBrightnessPanel MODULE
                brightness.h
                brightness.cpp
                plugin.h
                plugin.cpp
                ${QML_SOURCES})
qt5_use_modules(UbuntuBrightnessPanel Quick Qml DBus)

target_link_libraries(UbuntuBrightnessPanel uss-displays)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Brightness)
install(TARGETS uss-displays DESTINATION ${PLUGIN_MODULE_DIR})
install(TARGETS UbuntuBrightnessPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES brightness.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-brightness.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/brightness)
