# Set up and optionally install a mock plugin for ubuntu-settings-components.
#
# add_uss_mock(plugin version path)

macro(add_uss_mock PLUGIN VERSION PATH)
    set(single SEARCH_PATH TARGET_PREFIX)
    cmake_parse_arguments(MOCK "" "${single}" "" ${ARGN})
    if(NOT MOCK_SEARCH_PATH)
        set(MOCK_SEARCH_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(qmlfiles_dir ${CMAKE_CURRENT_BINARY_DIR})

    file(GLOB QMLFILES
        ${MOCK_SEARCH_PATH}/*.qml
        ${MOCK_SEARCH_PATH}/*.js
        ${MOCK_SEARCH_PATH}/*.jpg
        ${MOCK_SEARCH_PATH}/*.png
        ${MOCK_SEARCH_PATH}/*.sci
        ${MOCK_SEARCH_PATH}/*.svg
        ${MOCK_SEARCH_PATH}/*.qmltypes
        ${MOCK_SEARCH_PATH}/qmldir
    )

    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${qmlfiles_dir})

    # copy the files
    add_custom_target(${MOCK_TARGET_PREFIX}${PLUGIN}-qmlfiles ALL
                        COMMAND cp ${QMLFILES} ${qmlfiles_dir}
                        DEPENDS ${QMLFILES}
                        SOURCES ${QMLFILES}
    )
endmacro()

add_subdirectory(QMenuModel)
add_subdirectory(Ubuntu)
