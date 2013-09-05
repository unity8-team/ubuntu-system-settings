add_library(UbuntuSoundPanel MODULE plugin.h sound.h plugin.cpp sound.cpp)
qt5_use_modules(UbuntuSoundPanel Qml Quick)
