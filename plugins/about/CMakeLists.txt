include_directories(${ANDR_PROP_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GIO_INCLUDE_DIRS})
include_directories(${CLICK_INCLUDE_DIRS})
include_directories(${CLICK_INCLUDE_DIRS})
add_definitions(-DQT_NO_KEYWORDS)


set(QML_SOURCES
    EntryComponent.qml
    License.qml
    PageComponent.qml
    SingleValueStacked.qml
    Software.qml
    Storage.qml
    StorageBar.qml
    StorageItem.qml
    Version.qml
    DevMode.qml
    fakepkgslist.xml
)

add_library(UbuntuStorageAboutPanel MODULE
    plugin.cpp
    storageabout.cpp
    click.cpp
    plugin.h
    storageabout.h
    click.h
    ${QML_SOURCES} # So they show up in Qt designer.
)

qt5_use_modules(UbuntuStorageAboutPanel Qml Quick DBus)
target_link_libraries(UbuntuStorageAboutPanel
${ANDR_PROP_LDFLAGS} ${GLIB_LDFLAGS} ${GIO_LDFLAGS} ${CLICK_LDFLAGS} uss-systemimage)


set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/StorageAbout)
install(TARGETS UbuntuStorageAboutPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/about)
install(FILES settings-about.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES about.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
