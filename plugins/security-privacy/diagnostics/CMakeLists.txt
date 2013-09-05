add_library(UbuntuDiagnostics MODULE plugin.h diagnostics.h plugin.cpp diagnostics.cpp)
qt5_use_modules(UbuntuDiagnostics Qml Quick DBus)
