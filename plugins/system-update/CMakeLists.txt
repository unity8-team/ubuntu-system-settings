add_library(UbuntuUpdatePanel MODULE plugin.h update.h plugin.cpp update.cpp)
qt5_use_modules(UbuntuUpdatePanel Qml Quick DBus)
