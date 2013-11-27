
set(QML_SOURCES
    MainPage.qml
    OverlayImage.qml
    SwappableImage.qml
    Wallpapers.qml
    WallpaperGrid.qml
)
add_library(UbuntuBackgroundPanel MODULE
plugin.cpp background.cpp plugin.h background.h
${QML_SOURCES}) # So they show up in Qt designer.
qt5_use_modules(UbuntuBackgroundPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Background)
install(TARGETS UbuntuBackgroundPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/background)
install(FILES utilities.js homeoverlay.svg welcomeoverlay.svg import-image@18.png
        DESTINATION ${PLUGIN_QML_DIR}/background)
install(FILES settings-background.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES background.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
