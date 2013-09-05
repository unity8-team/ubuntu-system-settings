set(USS_SOURCES 
    debug.cpp
    i18n.cpp
    item-model.cpp
    main.cpp
    plugin-manager.cpp
    plugin.cpp
)

add_executable(system-settings ${USS_SOURCES})
qt5_use_modules(system-settings Core Qui Quick Qml)
