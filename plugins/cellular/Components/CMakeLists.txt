set(QML_SOURCES
    CellularSingleSim.qml
    data-helpers.js
    CellularDualSim.qml
    Sim.qml
    SimEditor.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular/Components)
