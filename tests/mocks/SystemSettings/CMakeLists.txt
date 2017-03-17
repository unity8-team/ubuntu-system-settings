include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
)

set(USS_SOURCES
    MockPluginManager.h
    MockPluginManager.cpp
    plugin.cpp
)

add_library(MockSystemSettings SHARED ${USS_SOURCES})
target_link_libraries(MockSystemSettings ${Qt5Core_LIBRARIES} ${Qt5Qml_LIBRARIES})

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR})
configure_file(qmldir ${CMAKE_CURRENT_BINARY_DIR}/qmldir)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ListItems)
configure_file(ListItems/qmldir ${CMAKE_CURRENT_BINARY_DIR}/ListItems/qmldir)
