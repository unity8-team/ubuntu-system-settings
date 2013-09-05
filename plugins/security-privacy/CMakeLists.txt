add_subdirectory(diagnostics)

add_library(UbuntuSecurityPrivacyPanel MODULE plugin.h securityprivacy.h plugin.cpp securityprivacy.cpp)
qt5_use_modules(UbuntuSecurityPrivacyPanel Qml Quick DBus)
