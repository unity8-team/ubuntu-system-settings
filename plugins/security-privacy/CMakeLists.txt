add_subdirectory(diagnostics)

add_library(UbuntuSecurityPrivacyPanel MODULE plugin.h securityprivacy.h plugin.cpp securityprivacy.cpp)
qt5_use_modules(UbuntuSecurityPrivacyPanel Qml Quick DBus)

set(QML_DIRS
    Dash.qml
    Location.qml
    LockSecurity.qml
    PageComponent.qml
    PhoneLocking.qml
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/SecurityPrivacy)
install(TARGETS UbuntuSecurityPrivacyPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/security-privacy)
install(FILES com.ubuntu.touch.AccountsService.SecurityPrivacy.xml
        DESTINATION share/dbus-1/interfaces)
install(FILES settings-security-privacy.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
