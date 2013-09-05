add_library(UbuntuStorageAboutPanel MODULE plugin.cpp storageabout.cpp plugin.h storageabout.h)
qt5_use_modules(UbuntuStorageAboutPanel Qml Quick)
target_link_libraries(UbuntuStorageAboutPanel ${ANDR_PROP_LIB})
