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
set(CMAKE_AUTOMOC ON)

pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)
pkg_search_module(LIBNM_GLIB REQUIRED libnm-glib)
pkg_search_module(ACCOUNTSSERVICE REQUIRED accountsservice)

set(PLUGIN_MANIFEST_DIR_BASE share/ubuntu/settings/system)
set(PLUGIN_MODULE_DIR_BASE ubuntu-system-settings)
set(PLUGIN_PRIVATE_MODULE_DIR_BASE "${PLUGIN_MODULE_DIR_BASE}/private")
set(PLUGIN_QML_DIR_BASE share/ubuntu/settings/system/qml-plugins)

set(PLUGIN_MANIFEST_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_MANIFEST_DIR_BASE}")
set(PLUGIN_MODULE_DIR "${LIBDIR}/${PLUGIN_MODULE_DIR_BASE}")
set(PLUGIN_QML_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_QML_DIR_BASE}")
set(PLUGIN_PRIVATE_MODULE_DIR "${LIBDIR}/${PLUGIN_PRIVATE_MODULE_DIR_BASE}")

set(ANDR_PROP_LIB -landroid-properties)

add_definitions(-DQT_NO_KEYWORDS)

add_subdirectory(lib)
include_directories(lib)
add_subdirectory(plugins)
add_subdirectory(src)
