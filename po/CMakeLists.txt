set(languages "ar;be;bg;bs;ca;de;el;en_AU;en_GB;eo;es;fa;fi;fr;gl;gu;he;hi;hr;id;it;ko;lv;my;nb;pl;pt_BR;pt;ru;sl;sv;tr;ug;uk;xh;zh_CN;zh_TW")
foreach(i ${languages})
#  GETTEXT_PROCESS_PO_FILES(${i} PO_FILES ALL INSTALL_DESTINATION share/locale PO_FILES "${i}.po")
  add_custom_command(OUTPUT ${i}.mo
  COMMAND ${MSGFMT_BIN} ${CMAKE_CURRENT_SOURCE_DIR}/${i}.po -o ${CMAKE_CURRENT_BINARY_DIR}/${i}.mo
  DEPENDS ${i}.po
  )
  add_custom_target(${i}gen ALL DEPENDS ${i}.mo)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${i}.mo 
  DESTINATION share/locale/${i}/LC_MESSAGES
  RENAME ubuntu-system-settings.mo)
endforeach()

