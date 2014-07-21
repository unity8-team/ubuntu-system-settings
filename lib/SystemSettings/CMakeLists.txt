include_directories(SYSTEM ${GIO_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS})

add_library(SystemSettings SHARED item-base.cpp item-base.h service.c)
target_link_libraries(SystemSettings ${GIO_LIBRARIES})
set_target_properties(SystemSettings PROPERTIES
VERSION 1.0.0
SOVERSION 1
)

qt5_use_modules(SystemSettings Core Gui Quick Qml)

install(TARGETS SystemSettings LIBRARY DESTINATION ${LIBDIR})
install(FILES item-base.h ItemBase plugin-interface.h PluginInterface ${CMAKE_CURRENT_BINARY_DIR}/service.h
DESTINATION include/SystemSettings)

set(SYSTEMSETTINGS_LIB SystemSettings)
configure_file(SystemSettings.pc.in SystemSettings.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SystemSettings.pc DESTINATION ${LIBDIR}/pkgconfig)

add_custom_command(OUTPUT service.h service.c COMMAND gdbus-codegen --interface-prefix com.ubuntu --generate-c-code service --c-namespace USS ${CMAKE_CURRENT_SOURCE_DIR}/com.ubuntu.Settings.xml DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/com.ubuntu.Settings.xml)
