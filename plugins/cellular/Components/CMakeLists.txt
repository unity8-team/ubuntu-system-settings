set(QML_SOURCES
    NoSim.qml
    SingleSim.qml
    MultiSim.qml
    CellularSingleSim.qml
    CellularMultiSim.qml
    data-helpers.js
    DefaultSim.qml
    Sim.qml
    SimEditor.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular/Components)

# add a phony target to get the files visible in Qt Creator.
add_custom_target(
    plugins_cellular_components_sources
    SOURCES ${QML_SOURCES}
)
