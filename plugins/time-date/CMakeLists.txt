include_directories(${GLIB_INCLUDE_DIRS} ${TIMEZONEMAP_INCLUDE_DIRS})

add_library(UbuntuTimeDatePanel MODULE plugin.h
  timedate.h
  timezonelocationmodel.h
  plugin.cpp
  timedate.cpp
  timezonelocationmodel.cpp
)
qt5_use_modules(UbuntuTimeDatePanel Qml Quick DBus)
target_link_libraries(UbuntuTimeDatePanel ${GLIB_LDFLAGS} ${TIMEZONEMAP_LDFLAGS})

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/TimeDate)
install(TARGETS UbuntuTimeDatePanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/timedate)

install(FILES time-date.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-time-date.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icon)
