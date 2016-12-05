include_directories(${CMAKE_CURRENT_BINARY_DIR} ../)

add_definitions(-DLAUNCHER_PLUGIN_QML_DIR="${PLUGIN_QML_DIR}/launcher")
add_library(launcher-plugin SHARED launcher-plugin.h launcher-plugin.cpp ../launcher.h ../launcher_impl.cpp)
qt5_use_modules(launcher-plugin Core Qml Widgets)
target_link_libraries(launcher-plugin SystemSettings)
install(TARGETS launcher-plugin DESTINATION ${PLUGIN_MODULE_DIR})
install(FILES GuAccessor.qml DESTINATION ${PLUGIN_QML_DIR}/launcher)
