file(GLOB SOURCECODE "${CMAKE_SOURCE_DIR}/plugins/*/*.qml" "${CMAKE_SOURCE_DIR}/src/qml/*.qml")
file(GLOB SETTINGSFILES "${CMAKE_SOURCE_DIR}/plugins/*/*.settings")

add_custom_command(OUTPUT settings.js
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/extractsettingsinfo ${SETTINGSFILES} -o ${CMAKE_CURRENT_BINARY_DIR}/settings.js
    DEPENDS ${SETTINGSFILES}
)

#add_custom_command(OUTPUT desktop.js
#  command some_script ${CMAKE_SOURCE_DIR}/ubuntu-system-settings.desktop ${CMAKE_CURRENT_BINARY_DIR}/desktop.js 
#  DEPENDS ${CMAKE_SOURCE_DIR}/ubuntu-system-settings.desktop
#)


add_custom_target(pot
    COMMAND ${XGETTEXT_BIN} -o ${CMAKE_CURRENT_SOURCE_DIR}/ubuntu-system-settings.pot
    -copyright=\"Canonical Ltd. \"
    --package-name ubuntu-system-settings
    --qt --c++ --add-comments=TRANSLATORS
    --keyword=tr --keyword=tr:1,2 --from-code=UTF-8
    ${SOURCECODE} ${DESKTOPFILE} settings.js
    DEPENDS ${SOURCECODE} settings.js #desktop.js
)

set(languages "ar;ast;be;bg;bs;ca;cs;de;el;en_AU;en_GB;eo;es;fa;fi;fr;gl;gu;he;hi;hr;hu;id;it;km;ko;lo;lv;ms;my;nb;pl;pt_BR;pt;ru;shn;sl;sv;tr;ug;uk;xh;zh_CN;zh_HK;zh_TW")
foreach(i ${languages})
  add_custom_command(OUTPUT ${i}.mo
  COMMAND ${MSGFMT_BIN} ${CMAKE_CURRENT_SOURCE_DIR}/${i}.po -o ${CMAKE_CURRENT_BINARY_DIR}/${i}.mo
  DEPENDS ${i}.po
  )
  add_custom_target(${i}gen ALL DEPENDS ${i}.mo)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${i}.mo 
  DESTINATION share/locale/${i}/LC_MESSAGES
  RENAME ubuntu-system-settings.mo)
endforeach()

