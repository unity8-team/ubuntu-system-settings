include_directories(${CMAKE_CURRENT_BINARY_DIR} ../../../plugins/security-privacy)
add_definitions(-DTESTS)

add_executable(tst-password
    tst_password.cpp
    ../../../plugins/security-privacy/securityprivacy.cpp
)

qt5_use_modules(tst-password Core DBus Test)
target_link_libraries (tst-password gcrypt uss-accountsservice)
add_test(NAME tst-password COMMAND sh -c "rm -rf ${CMAKE_CURRENT_BINARY_DIR}/home && mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/home && env HOME=${CMAKE_CURRENT_BINARY_DIR}/home USER=testuser ${CMAKE_CURRENT_BINARY_DIR}/tst-password")
