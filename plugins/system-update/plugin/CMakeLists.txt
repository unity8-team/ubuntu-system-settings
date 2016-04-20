SET (CMAKE_AUTOMOC ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(update-plugin SHARED
            update-plugin.cpp
            # ../network.cpp
            # ../network.h
            # ../update.cpp
            # ../update.h
            # ../update_manager.cpp
            # ../update_manager.h
            # ../system_update.cpp
            # ../system_update.h
            # ../download_tracker.h
            # ../download_tracker.cpp
            ../updatemanager.cpp
            ../clickupdatechecker.cpp
            ../clickupdatemetadata.cpp
            ../clickapiproto.cpp
            ../clickapicache.cpp
            ../helpers.cpp
)

qt5_use_modules(update-plugin Core Qml Quick Network DBus)
include_directories(/usr/include/apt-pkg/)
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

target_link_libraries(update-plugin
      SystemSettings
      uss-systemimage
      apt-pkg
      ${UBUNTUONEAUTH_LDFLAGS}
)

install(TARGETS update-plugin DESTINATION ${PLUGIN_MODULE_DIR})
