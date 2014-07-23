configure_file(session-settings-service.conf.in session-settings-service.conf @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/session-settings-service.conf DESTINATION ${UPSTART_INSTALL_DIR})
