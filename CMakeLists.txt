project(ubuntu-system-settings C CXX)
cmake_minimum_required(VERSION 2.8.10)

if(${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
   message(FATAL_ERROR "In-tree build attempt detected, aborting. Set your build dir outside your source dir, delete CMakeCache.txt from source root and try again.")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower) # Build types should always be lowercase but sometimes they are not.

include(FindPkgConfig)
include(GNUInstallDirs)
set(LIBDIR ${CMAKE_INSTALL_LIBDIR})
enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-permissive -pedantic -Wall -Wextra")
find_package(Qt5Widgets REQUIRED)
# Workaround for https://bugreports.qt-project.org/browse/QTBUG-29987
set(QT_IMPORTS_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/qml")

set(PLUGIN_MANIFEST_DIR_BASE share/ubuntu/settings/system)
set(PLUGIN_MODULE_DIR_BASE ubuntu-system-settings)
set(PLUGIN_PRIVATE_MODULE_DIR_BASE "${PLUGIN_MODULE_DIR_BASE}/private")
set(PLUGIN_QML_DIR_BASE share/ubuntu/settings/system/qml-plugins)

set(PLUGIN_MANIFEST_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_MANIFEST_DIR_BASE}")
set(PLUGIN_MODULE_DIR "${LIBDIR}/${PLUGIN_MODULE_DIR_BASE}")
set(PLUGIN_QML_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_QML_DIR_BASE}")
set(PLUGIN_PRIVATE_MODULE_DIR "${LIBDIR}/${PLUGIN_PRIVATE_MODULE_DIR_BASE}")

include_directories(lib)
add_subdirectory(src)
