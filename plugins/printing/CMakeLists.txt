add_definitions(-DQT_NO_KEYWORDS)

set(QML_SOURCES
    printer-sub-pages/CopiesAndPages.qml
    printer-sub-pages/GeneralSettings.qml
    printer-sub-pages/Policies.qml
    printer-sub-pages/Status.qml

    AddPrinter.qml
    PageComponent.qml
    Printer.qml
    PrinterConnection.qml
    Setting.qml
)

# We need a dummy target so the QML files show up in Qt Creator
# If this plugin gets some C++ sources, remove this.
add_custom_target(printing-holder
COMMAND echo This is just a dummy.
SOURCES ${QML_SOURCES})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Printing)

install(FILES qmldir.in DESTINATION ${PLUG_DIR} RENAME qmldir)
install(FILES printing.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES printing.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/printing)
