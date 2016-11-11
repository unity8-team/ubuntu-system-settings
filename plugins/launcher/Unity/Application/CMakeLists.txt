include_directories(
    ${CMAKE_SOURCE_DIR}/src/platforms/mirserver
    ${CMAKE_SOURCE_DIR}/src/common
)

include_directories(
    SYSTEM
    ${GLIB_INCLUDE_DIRS}
    ${GIO_INCLUDE_DIRS}
    ${GIO_UNIX_INCLUDE_DIRS}
    ${MIRSERVER_INCLUDE_DIRS}
    ${MIRRENDERERGLDEV_INCLUDE_DIRS}
    ${PROCESS_CPP_INCLUDE_DIRS}
    ${UBUNTU_PLATFORM_API_INCLUDE_DIRS}
    ${UBUNTU_APP_LAUNCH_INCLUDE_DIRS}
    ${GSETTINGS_QT_INCLUDE_DIRS}
    ${CGMANAGER_INCLUDE_DIRS}

    ${LTTNG_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Qml_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
)

# We have to remove -pedantic in order to compile tracepoints.c
string (REPLACE " -pedantic " " " CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
# Needed to compile tracepoints in C99 mode.
add_definitions(-DBYTE_ORDER=__BYTE_ORDER)

set(QMLMIRPLUGIN_SRC
    application_manager.cpp
    application.cpp
    cgmanager.cpp
    ../../../common/abstractdbusservicemonitor.cpp
    ../../../common/debughelpers.cpp
    dbusfocusinfo.cpp
    plugin.cpp
    mirsurfacemanager.cpp
    mirfocuscontroller.cpp
    mirsurface.cpp
    mirsurfaceinterface.h
    mirsurfaceitem.cpp
    mirsurfacelistmodel.cpp
    mirbuffersgtexture.cpp
    proc_info.cpp
    session.cpp
    sessionmanager.cpp
    sharedwakelock.cpp
    upstart/applicationinfo.cpp
    upstart/taskcontroller.cpp
    timer.cpp
    timesource.cpp
    tracepoints.c
    settings.cpp
# We need to run moc on these headers
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/ApplicationInfoInterface.h
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/ApplicationManagerInterface.h
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/Mir.h
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/MirFocusControllerInterface.h
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/MirSurfaceInterface.h
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/MirSurfaceItemInterface.h
    ${APPLICATION_API_INCLUDEDIR}/unity/shell/application/MirSurfaceListInterface.h
# Feed the automoc monster
    session_interface.h
    applicationinfo.h
    taskcontroller.h
    settings_interface.h
    )

add_library(unityapplicationplugin SHARED
    ${QMLMIRPLUGIN_SRC}
)

target_link_libraries(
    unityapplicationplugin

    ${CMAKE_THREAD_LIBS_INIT}

    ${GLIB_LDFLAGS}
    ${UBUNTU_PLATFORM_API_LDFLAGS}
    ${MIRSERVER_LDFLAGS}
    ${PROCESS_CPP_LDFLAGS}
    ${UBUNTU_APP_LAUNCH_LDFLAGS}
    ${LTTNG_LDFLAGS}
    ${GSETTINGS_QT_LDFLAGS}

    ${GL_LIBRARIES}

    Qt5::Core
    Qt5::DBus
    Qt5::Qml
    Qt5::Quick

    qpa-mirserver
)

# Generate tracepoints.c and .h from tracepoints.tp
include(UseLttngGenTp)
add_lttng_gen_tp(NAME tracepoints)


# install
add_qml_plugin(Unity.Application 0.1 Unity/Application TARGETS unityapplicationplugin)
install(FILES com.canonical.qtmir.gschema.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)
