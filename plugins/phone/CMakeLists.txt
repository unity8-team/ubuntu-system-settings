include_directories(${OFONO_INCLUDE_DIRS})

#include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_library(UbuntuPhonePanel MODULE
    connman.h
    simservice.h
    simmanager.h
    networkoperator.h
    networkregistration.h
    plugin.cpp
    connman.cpp
    simservice.cpp
    simmanager.cpp
    networkoperator.cpp
    networkregistration.cpp
    plugin.h
)
qt5_use_modules(UbuntuPhonePanel Qml Quick DBus)
target_link_libraries(UbuntuPhonePanel ${OFONO_LDFLAGS})

set(QML_SOURCES
    CallForwarding.qml
    CallWaiting.qml
    PageComponent.qml
    ServiceInfo.qml
    Services.qml
)
set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Phone)
install(TARGETS UbuntuPhonePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/about)
