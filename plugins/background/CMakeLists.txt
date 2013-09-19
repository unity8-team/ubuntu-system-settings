add_library(UbuntuBackgroundPanel MODULE plugin.cpp background.cpp plugin.h background.h)
qt5_use_modules(UbuntuBackgroundPanel Qml Quick DBus)

set(QML_SOURCES
    ChangeImage.qml
    MainPage.qml
    SwappableImage.qml
#    UbuntuSwappableImage.qml
)

install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/background)
install(FILES settings-background.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
