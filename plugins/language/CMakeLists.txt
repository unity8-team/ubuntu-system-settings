pkg_search_module(GD3 REQUIRED gnome-desktop-3.0)
include_directories(${GD3_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${ACCOUNTSSERVICE_INCLUDE_DIRS} ${ICU_INCLUDE_DIRS})
add_definitions(-DQT_NO_KEYWORDS)

set(QML_SOURCES
    DisplayLanguage.qml
    KeyboardLayoutItem.qml
    KeyboardLayouts.qml
    PageComponent.qml
    PageHardwareKeyboard.qml
    SpellChecking.qml
    SubsetView.qml
)


add_library(UbuntuLanguagePlugin MODULE
 keyboard-layout.cpp language-plugin.cpp plugin.cpp subset-model.cpp onscreenkeyboard-plugin.cpp hardwarekeyboard-plugin.cpp
 keyboard-layout.h language-plugin.h plugin.h subset-model.h onscreenkeyboard-plugin.h hardwarekeyboard-plugin.h
 ${QML_SOURCES})
qt5_use_modules(UbuntuLanguagePlugin Qml Quick DBus)
target_link_libraries(UbuntuLanguagePlugin uss-accountsservice uss-sessionservice ${GD3_LDFLAGS} ${GLIB_LDFLAGS} ${GIO_LDFLAGS} ${ACCOUNTSSERVICE_LDFLAGS} ${ICU_LDFLAGS})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/LanguagePlugin)
install(TARGETS UbuntuLanguagePlugin DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/language)
install(FILES settings-language.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES language.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
