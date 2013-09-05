add_library(UbuntuBackgroundPanel MODULE plugin.cpp background.cpp plugin.h background.h)
qt5_use_modules(UbuntuBackgroundPanel Qml Quick DBus)
