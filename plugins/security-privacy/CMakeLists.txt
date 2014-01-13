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
target_link_libraries (UbuntuSecurityPrivacyPanel uss-accountsservice)
install(TARGETS UbuntuSecurityPrivacyPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/security-privacy)

install(FILES settings-security-privacy.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES security-privacy.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
