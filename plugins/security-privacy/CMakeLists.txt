add_subdirectory(diagnostics)

set(QML_SOURCES
    AppAccess.qml
    Dash.qml
    Location.qml
    LockSecurity.qml
    PageComponent.qml
    PhoneLocking.qml
)

add_library(UbuntuSecurityPrivacyPanel MODULE
    plugin.cpp
    plugin.h
    securityprivacy.cpp
    securityprivacy.h
    trust-store-model.cpp
    trust-store-model.h
    ${QML_SOURCES}
)
qt5_use_modules(UbuntuSecurityPrivacyPanel Qml Quick DBus)

pkg_check_modules(TRUST_STORE REQUIRED trust-store)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${TRUST_STORE_INCLUDE_DIRS}
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/SecurityPrivacy)
target_link_libraries (UbuntuSecurityPrivacyPanel
    uss-accountsservice
    ${TRUST_STORE_LDFLAGS}
)
install(TARGETS UbuntuSecurityPrivacyPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/security-privacy)

install(FILES settings-security-privacy.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES security-privacy.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
