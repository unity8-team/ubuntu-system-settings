set(QML_SOURCES
    PageComponent.qml
    None.qml
    Connected.qml
    ItemTitle.qml
    TapArea.qml
    PrimaryButtonSelector.qml
    SectionHeader.qml
)

set(PANEL_SOURCES
    mouse.cpp
    mouse.h
    plugin.cpp
    plugin.h
    ${QML_SOURCES}
)

add_definitions(-DQT_NO_KEYWORDS)
add_library(UbuntuMousePanel MODULE ${PANEL_SOURCES} ${QML_SOURCES})
qt5_use_modules(UbuntuMousePanel Qml Quick DBus)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${ACCOUNTSSERVICE_INCLUDE_DIRS}
                    ${GOBJECT_INCLUDE_DIRS})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Mouse)
target_link_libraries (UbuntuMousePanel
    uss-accountsservice
    ${ACCOUNTSSERVICE_LDFLAGS}
    ${GOBJECT_LDFLAGS}
    ${GLIB_LDFLAGS}
)
install(TARGETS UbuntuMousePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(mouse-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

install(FILES mouse.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-mouse.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/mouse)
