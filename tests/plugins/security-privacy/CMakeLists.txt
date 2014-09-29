set(XVFB_CMD xvfb-run -a -s "-screen 0 640x480x24")
include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/security-privacy ${GLIB_INCLUDE_DIRS})
add_definitions(-DTESTS)

add_executable(tst-trust-store-model
    tst_trust_store_model.cpp
    ../../../plugins/security-privacy/trust-store-model.cpp
)
target_link_libraries (tst-trust-store-model ${GLIB_LDFLAGS})
qt5_use_modules(tst-trust-store-model Core Gui DBus Qml Test)
add_test(NAME tst-trust-store-model COMMAND ${XVFB_CMD} ${CMAKE_CURRENT_BINARY_DIR}/tst-trust-store-model)
