set(QML_SOURCES
    PageComponent.qml
    SilentModeWarning.qml
    SoundsList.qml
)

add_library(UbuntuSoundPanel MODULE plugin.h sound.h plugin.cpp sound.cpp
${QML_SOURCES})
target_link_libraries(UbuntuSoundPanel uss-accountsservice)
qt5_use_modules(UbuntuSoundPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Sound)
install(TARGETS UbuntuSoundPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/sound)

install(FILES com.ubuntu.touch.AccountsService.Sound.xml
        DESTINATION share/dbus-1/interfaces)
install(CODE "
execute_process(COMMAND mkdir -p \"\$ENV{DESTDIR}${ACCOUNTS_IFACE_DIR}\")
execute_process(COMMAND ln -sf ../../dbus-1/interfaces/com.ubuntu.touch.AccountsService.Sound.xml \"\$ENV{DESTDIR}${ACCOUNTS_IFACE_DIR}\")
       ")

install(FILES sound.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-sounds.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES utilities.js DESTINATION ${PLUGIN_QML_DIR}/sound)
