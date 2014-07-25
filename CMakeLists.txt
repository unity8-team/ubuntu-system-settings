project(ubuntu-system-settings C CXX)
cmake_minimum_required(VERSION 2.8.10)

if(${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
   message(FATAL_ERROR "In-tree build attempt detected, aborting. Set your build dir outside your source dir, delete CMakeCache.txt from source root and try again.")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower) # Build types should always be lowercase but sometimes they are not.

set(PROJECT_VERSION 0.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(EnableCoverageReport)
include(FindPkgConfig)
include(GNUInstallDirs)
set(LIBDIR ${CMAKE_INSTALL_LIBDIR})
enable_testing()

if(cmake_build_type_lower MATCHES coverage)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage" )
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage" )
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage" )

  # We add -g when building with coverage so valgrind reports line numbers.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fno-permissive -pedantic -Wall -Wextra")
find_package(Qt5Widgets REQUIRED)
# Workaround for https://bugreports.qt-project.org/browse/QTBUG-29987
set(QT_IMPORTS_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/qml")
set(CMAKE_AUTOMOC ON)

pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0 gio-unix-2.0)
pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)
pkg_search_module(LIBNM_GLIB REQUIRED libnm-glib)
pkg_search_module(ACCOUNTSSERVICE REQUIRED accountsservice)
pkg_search_module(TIMEZONEMAP REQUIRED timezonemap)
pkg_search_module(ICU REQUIRED icu-i18n)
pkg_search_module(ANDR_PROP libandroid-properties)
pkg_check_modules(QTDBUSMOCK REQUIRED libqtdbusmock-1 REQUIRED)
pkg_check_modules(QTDBUSTEST REQUIRED libqtdbustest-1 REQUIRED)

find_program(XGETTEXT_BIN xgettext)
find_program(MSGFMT_BIN msgfmt)

set(PLUGIN_MANIFEST_DIR_BASE share/ubuntu/settings/system)
set(PLUGIN_MODULE_DIR_BASE ubuntu-system-settings)
set(PLUGIN_PRIVATE_MODULE_DIR_BASE "${PLUGIN_MODULE_DIR_BASE}/private")
set(PLUGIN_QML_DIR_BASE share/ubuntu/settings/system/qml-plugins)

set(PLUGIN_MANIFEST_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_MANIFEST_DIR_BASE}")
set(PLUGIN_MODULE_DIR "${CMAKE_INSTALL_PREFIX}/${LIBDIR}/${PLUGIN_MODULE_DIR_BASE}")
set(PLUGIN_QML_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_QML_DIR_BASE}")
set(PLUGIN_PRIVATE_MODULE_DIR "${CMAKE_INSTALL_PREFIX}/${LIBDIR}/${PLUGIN_PRIVATE_MODULE_DIR_BASE}")
set(SETTINGS_SHARE_DIR "${CMAKE_INSTALL_PREFIX}/${PLUGIN_MANIFEST_DIR_BASE}")

SET(CMAKE_INSTALL_RPATH "${PLUGIN_MODULE_DIR}")

add_subdirectory(po)
add_subdirectory(schema)
add_subdirectory(lib)
include_directories(lib)
include_directories(src)
add_subdirectory(plugins)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(wizard)

configure_file(ubuntu-system-settings.desktop.in ubuntu-system-settings.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ubuntu-system-settings.desktop DESTINATION share/applications)
install(FILES ubuntu-system-settings.url-dispatcher DESTINATION share/url-dispatcher/urls)
install(FILES screenshot.png DESTINATION ${SETTINGS_SHARE_DIR})

if(cmake_build_type_lower MATCHES coverage)
  ENABLE_COVERAGE_REPORT(TARGETS system-settings FILTER /usr/include ${CMAKE_SOURCE_DIR}/tests/* ${CMAKE_BINARY_DIR}/*)
endif()
