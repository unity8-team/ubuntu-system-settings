include_directories(SYSTEM ${GIO_INCLUDE_DIRS} ${ACCOUNTSSERVICE_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS} ${ACCOUNTSSERVICE_LIBRARY_DIRS})

add_executable(service-bin server.c)
set_target_properties(service-bin PROPERTIES OUTPUT_NAME system-settings-service)
target_link_libraries(service-bin ${GIO_LIBRARIES} ${ACCOUNTSSERVICE_LIBRARIES})
install(TARGETS service-bin RUNTIME DESTINATION ${SERVICE_INSTALL_DIR})

add_custom_command(OUTPUT server.c COMMAND valac -C --enable-experimental-non-null --vapidir=. --pkg gio-2.0 --pkg posix --pkg accountsservice ${CMAKE_CURRENT_SOURCE_DIR}/server.vala DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/server.vala accountsservice.vapi)
add_custom_command(OUTPUT accountsservice.vapi COMMAND vapigen --pkg gio-2.0 --library accountsservice /usr/share/gir-1.0/AccountsService-1.0.gir)
