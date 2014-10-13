set(QML_SOURCES
    DataMultiSim.qml
    DefaultSim.qml
    MultiSim.qml
    NoSim.qml
    RadioSingleSim.qml
    Sim.qml
    SimEditor.qml
    SingleSim.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular/Components)
