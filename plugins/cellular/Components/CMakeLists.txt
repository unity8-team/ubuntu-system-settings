set(QML_SOURCES
    CarrierSelection.qml
    CellularSingleSim.qml
    data-helpers.js
    CellularDualSim.qml
    Sim.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular/Components)
