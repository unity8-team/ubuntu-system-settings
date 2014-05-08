include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(update-plugin SHARED update-plugin.h update-plugin.cpp ../update.cpp ../update.h
../system_update.cpp ../system_update.h ../network/network.h ../network/network.cpp
../update_manager.h ../update_manager.cpp)
qt5_use_modules(update-plugin Core Qml Quick DBus)
include_directories(/usr/include/apt-pkg/)
target_link_libraries(update-plugin SystemSettings apt-pkg)
install(TARGETS update-plugin DESTINATION ${PLUGIN_MODULE_DIR})
