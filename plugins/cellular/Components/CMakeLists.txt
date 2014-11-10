set(QML_SOURCES
    DataMultiSim.qml
    DefaultSim.qml
    MultiSim.qml
    NoSim.qml
    OperatorLabel.qml
    RadioSingleSim.qml
    Sim.qml
    SimEditor.qml
    SingleSim.qml
)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/cellular/Components)

# add a phony target to get the files visible in Qt Creator.
add_custom_target(
    plugins_cellular_components_sources
    SOURCES ${QML_SOURCES}
)
