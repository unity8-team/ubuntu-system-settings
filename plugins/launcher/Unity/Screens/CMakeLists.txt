include_directories(
    ${CMAKE_SOURCE_DIR}/src/platforms/mirserver
)

include_directories(
    SYSTEM
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${MIRSERVER_INCLUDE_DIRS}
    )

set(SCREENSPLUGIN_SRC
    plugin.cpp
    screens.cpp
    qquickscreenwindow.cpp
    )

add_library(unityscreensplugin SHARED
    ${SCREENSPLUGIN_SRC}
)

target_link_libraries(
    unityscreensplugin

    qpa-mirserver

    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
)

# install
add_qml_plugin(Unity.Screens 0.1 Unity/Screens TARGETS unityscreensplugin)
