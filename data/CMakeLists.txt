configure_file(system-settings-service.conf.in system-settings-service.conf @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/system-settings-service.conf DESTINATION ${UPSTART_INSTALL_DIR})
