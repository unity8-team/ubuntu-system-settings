include_directories(${OFONO_INCLUDE_DIRS})

set(QML_SOURCES
    CallForwarding.qml
    CallWaiting.qml
    PageComponent.qml
    ServiceInfo.qml
    Services.qml
)

add_library(UbuntuPhonePanel MODULE
    connman.h
    simservice.h
    simmanager.h
    networkoperator.h
    networkregistration.h
    radiosettings.h
    plugin.cpp
    connman.cpp
    simservice.cpp
    simmanager.cpp
    networkoperator.cpp
    networkregistration.cpp
    radiosettings.cpp
    plugin.h
    ${QML_SOURCES}
)
qt5_use_modules(UbuntuPhonePanel Qml Quick DBus)
target_link_libraries(UbuntuPhonePanel ${OFONO_LDFLAGS})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Phone)
install(TARGETS UbuntuPhonePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/phone)
install(FILES settings-phone.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES phone.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
