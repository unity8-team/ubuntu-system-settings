add_library(SystemSettings SHARED item-base.cpp item-base.h)
set_target_properties(SystemSettings PROPERTIES
VERSION 1.0
SOVERSION 1
)
qt5_use_modules(SystemSettings Core Gui Quick Qml)

install(TARGET SystemSettings LIBRARY DESTINATION ${LIBDIR})
install(FILES item-base.h ItemBase plugin-interface.h PluginInterface
DESTINATION include/SystemSettings)
