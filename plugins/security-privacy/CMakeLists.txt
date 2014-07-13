add_subdirectory(diagnostics)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/SecurityPrivacy)

set(HELPER_SOURCES
    polkitlistener.cpp
    helper.cpp
)
add_executable(UbuntuSecurityPrivacyHelper ${HELPER_SOURCES})
set_target_properties(UbuntuSecurityPrivacyHelper PROPERTIES
    INCLUDE_DIRECTORIES "${POLKIT_AGENT_INCLUDE_DIRS}"
    COMPILE_DEFINITIONS "POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE"
)
target_link_libraries(UbuntuSecurityPrivacyHelper ${POLKIT_AGENT_LDFLAGS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ACCOUNTSSERVICE_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
add_definitions(-DHELPER_EXEC="${PLUG_DIR}/UbuntuSecurityPrivacyHelper" -DQT_NO_KEYWORDS)

set(QML_SOURCES
    Dash.qml
    Location.qml
    LockSecurity.qml
    PageComponent.qml
    PhoneLocking.qml
)

set(PANEL_SOURCES
    plugin.cpp
    securityprivacy.cpp
)

add_library(UbuntuSecurityPrivacyPanel MODULE ${PANEL_SOURCES} ${QML_SOURCES})
qt5_use_modules(UbuntuSecurityPrivacyPanel Qml Quick DBus)

target_link_libraries (UbuntuSecurityPrivacyPanel uss-accountsservice ${ACCOUNTSSERVICE_LDFLAGS})
install(TARGETS UbuntuSecurityPrivacyPanel UbuntuSecurityPrivacyHelper DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/security-privacy)

install(FILES settings-security-privacy.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES security-privacy.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
