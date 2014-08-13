set(QML_SOURCES
    CellularSingleSim.qml
    CellularDualSim.qml
    data-helpers.js
    DefaultSim.qml
    Sim.qml
    SimEditor.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular/Components)
