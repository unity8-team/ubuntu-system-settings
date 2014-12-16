SET (CMAKE_AUTOMOC ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
	update-plugin.cpp
	../network.cpp
	../update.cpp
	../update_manager.cpp
	../system_update.cpp
	../download_tracker.cpp
)

set(HEADERS
	update-plugin.h
	../network.h
	../update.h
	../update_manager.h
	../system_update.h
        ../download_tracker.h
)

add_library(update-plugin SHARED
	${HEADERS}
	${SOURCES}
)

qt5_use_modules(update-plugin Core Qml Quick Network DBus)
include_directories(/usr/include/apt-pkg/)
pkg_check_modules(UBUNTUONEAUTH REQUIRED ubuntuoneauth-2.0)
add_definitions(${UBUNTUONEAUTH_CFLAGS} ${UBUNTUONEAUTH_CFLAGS_OTHER})

target_link_libraries(update-plugin SystemSettings apt-pkg ${UBUNTUONEAUTH_LDFLAGS})
install(TARGETS update-plugin DESTINATION ${PLUGIN_MODULE_DIR})
