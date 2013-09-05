include_directories(${GLIB_INCLUDE_DIRS} ${ACCOUNTSSERVICE_INCLUDE_DIRS})
add_definitions(-DQT_NO_KEYWORDS)


add_library(UbuntuLanguagePlugin MODULE language-plugin.cpp language-plugin.h plugin.cpp plugin.h)
qt5_use_modules(UbuntuLanguagePlugin Qml Quick DBus)
target_link_libraries(UbuntuLanguagePlugin ${GLIB_LDFLAGS} ${ACCOUNTSSERVICE_LDFLAGS})
