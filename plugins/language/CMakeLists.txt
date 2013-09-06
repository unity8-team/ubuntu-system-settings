include_directories(${GLIB_INCLUDE_DIRS} ${ACCOUNTSSERVICE_INCLUDE_DIRS})
add_definitions(-DQT_NO_KEYWORDS)


add_library(UbuntuLanguagePlugin MODULE language-plugin.cpp language-plugin.h plugin.cpp plugin.h)
qt5_use_modules(UbuntuLanguagePlugin Qml Quick DBus)
target_link_libraries(UbuntuLanguagePlugin ${GLIB_LDFLAGS} ${ACCOUNTSSERVICE_LDFLAGS})

set(QML_SOURCES
    DisplayLanguage.qml
    KeyboardLayouts.qml
    PageComponent.qml
    SpellChecking.qml
)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/LanguagePlugin)
install(TARGETS UbuntuLanguagePlugin DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/about)
