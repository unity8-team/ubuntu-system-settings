add_library(UbuntuStorageAboutPanel MODULE plugin.cpp storageabout.cpp click.cpp plugin.h storageabout.h click.h)
qt5_use_modules(UbuntuStorageAboutPanel Qml Quick)
target_link_libraries(UbuntuStorageAboutPanel ${ANDR_PROP_LIB})

set(QML_SOURCES
    EntryComponent.qml
    License.qml
    PageComponent.qml
    Software.qml
    Storage.qml
    StorageBar.qml
    StorageItem.qml
    fakepkgslist.xml
)
set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/StorageAbout)
install(TARGETS UbuntuStorageAboutPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/about)
install(FILES settings-about.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
