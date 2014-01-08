add_subdirectory(diagnostics)

set(QML_SOURCES
    Dash.qml
    Location.qml
    LockSecurity.qml
    PageComponent.qml
    PhoneLocking.qml
)

add_library(UbuntuSecurityPrivacyPanel MODULE plugin.h securityprivacy.h plugin.cpp securityprivacy.cpp
${QML_SOURCES})
qt5_use_modules(UbuntuSecurityPrivacyPanel Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/SecurityPrivacy)
install(TARGETS UbuntuSecurityPrivacyPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/security-privacy)
install(FILES com.ubuntu.touch.AccountsService.SecurityPrivacy.xml
        DESTINATION share/dbus-1/interfaces)
install(CODE "
execute_process(COMMAND mkdir -p \"\$ENV{DESTDIR}${ACCOUNTS_IFACE_DIR}\")
execute_process(COMMAND ln -sf ../../dbus-1/interfaces/com.ubuntu.touch.AccountsService.SecurityPrivacy.xml \"\$ENV{DESTDIR}${ACCOUNTS_IFACE_DIR}\")
       ")
install(FILES settings-security-privacy.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES security-privacy.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
