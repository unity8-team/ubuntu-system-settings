include_directories(${GLIB_INCLUDE_DIRS})
install(FILES cellular.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)

set(QML_SOURCES
    ChooseCarrier.qml
    PageComponent.qml
)

add_library(UbuntuCellularPanel MODULE
    plugin.h
    plugin.cpp
    radiosettings.h
    radiosettings.cpp
    cellular.h
    cellular.cpp
    ${QML_SOURCES})

qt5_use_modules(UbuntuCellularPanel Quick Qml DBus)
target_link_libraries(UbuntuCellularPanel ${GLIB_LDFLAGS})

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(cellular-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Cellular)
install(TARGETS UbuntuCellularPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})

install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular)
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
