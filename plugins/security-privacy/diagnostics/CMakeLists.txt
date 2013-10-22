set(QML_SOURCES
    PageComponent.qml
    DiagnosticsCheckEntry.qml
)

add_library(UbuntuDiagnostics MODULE plugin.h diagnostics.h plugin.cpp diagnostics.cpp
${QML_SOURCES})
qt5_use_modules(UbuntuDiagnostics Qml Quick DBus)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Diagnostics)
install(TARGETS UbuntuDiagnostics DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/diagnostics)
