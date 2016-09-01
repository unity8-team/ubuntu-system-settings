# Set up and optionally install a mock plugin for ubuntu-system-setting.
# add_uss_mock(plugin version path)

macro(add_uss_mock PLUGIN VERSION PATH)
    cmake_parse_arguments(MOCK "" "${single}" "" ${ARGN})

    if(NOT QMLFILES_SEARCH_PATH)
        set(QMLFILES_SEARCH_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(qmlfiles_dir ${CMAKE_CURRENT_BINARY_DIR})

    file(GLOB QMLFILES
        ${QMLFILES_SEARCH_PATH}/*.qml
        ${QMLFILES_SEARCH_PATH}/*.js
        ${QMLFILES_SEARCH_PATH}/*.jpg
        ${QMLFILES_SEARCH_PATH}/*.png
        ${QMLFILES_SEARCH_PATH}/*.sci
        ${QMLFILES_SEARCH_PATH}/*.svg
        ${QMLFILES_SEARCH_PATH}/*.qmltypes
        ${QMLFILES_SEARCH_PATH}/qmldir
    )

    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${qmlfiles_dir})

    # copy the files
    add_custom_target(${QMLFILES_TARGET_PREFIX}${PLUGIN}-qmlfiles ALL
                        COMMAND cp ${QMLFILES} ${qmlfiles_dir}
                        DEPENDS ${QMLFILES}
                        SOURCES ${QMLFILES}
    )
endmacro()

add_subdirectory(QMenuModel)
add_subdirectory(Ubuntu)
