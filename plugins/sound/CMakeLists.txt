add_library(UbuntuSoundPanel MODULE plugin.h sound.h plugin.cpp sound.cpp)
qt5_use_modules(UbuntuSoundPanel Qml Quick)

set(QML_SOURCES
    PageComponent.qml
    SilentModeWarning.qml
    SoundsList.qml
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Sound)
install(TARGETS UbuntuSoundPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/sound)

install(FILES sound.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-sounds.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
