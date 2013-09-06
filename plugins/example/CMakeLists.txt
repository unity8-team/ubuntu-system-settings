include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(example-plugin MODULE example-plugin.cpp example-plugin.h)
qt5_use_modules(example-plugin Qml Core)
target_link_libraries(example-plugin SystemSettings)

set(QML_SOURCES PageComponent.qml)
install(FILES example.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/example)
