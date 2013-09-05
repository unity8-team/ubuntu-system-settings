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
