include_directories(${OFONO_INCLUDE_DIRS})

set(QML_SOURCES
    ChooseCarrier.qml
    PageComponent.qml
)

add_library(UbuntuCellularPanel MODULE
    plugin.h
    plugin.cpp
    radiosettings.h
    radiosettings.cpp
    ${QML_SOURCES})

qt5_use_modules(UbuntuCellularPanel Quick Qml DBus)
target_link_libraries(UbuntuCellularPanel ${OFONO_LDFLAGS})
# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(cellular-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Cellular)
install(TARGETS UbuntuCellularPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES cellular.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular)
install(FILES settings-cellular.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
