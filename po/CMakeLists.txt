file(GLOB CPPFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
                   "${CMAKE_SOURCE_DIR}/plugins/*/*.cpp"
                   "${CMAKE_SOURCE_DIR}/src/*.cpp"
                   "${CMAKE_SOURCE_DIR}/wizard/*.cpp")
file(GLOB QMLFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
                   "${CMAKE_SOURCE_DIR}/plugins/*/*.qml"
                   "${CMAKE_SOURCE_DIR}/plugins/*/*/*.qml"
                   "${CMAKE_SOURCE_DIR}/src/qml/*.qml"
                   "${CMAKE_SOURCE_DIR}/wizard/qml/*/*.qml")
file(GLOB SETTINGSFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
                        "${CMAKE_SOURCE_DIR}/plugins/*/*.settings")
file(GLOB PYFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
                        "${CMAKE_SOURCE_DIR}/push-helper/*.py")

add_custom_command(OUTPUT settings.js
    COMMAND ./extractsettingsinfo ${SETTINGSFILES} -o ${CMAKE_CURRENT_BINARY_DIR}/settings.js
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS ${SETTINGSFILES}
)

#add_custom_command(OUTPUT desktop.js
#  command some_script ${CMAKE_SOURCE_DIR}/ubuntu-system-settings.desktop ${CMAKE_CURRENT_BINARY_DIR}/desktop.js 
#  DEPENDS ${CMAKE_SOURCE_DIR}/ubuntu-system-settings.desktop
#)

add_custom_target(pot-qml
    COMMAND ${XGETTEXT_BIN} -o ubuntu-system-settings.pot
                            --copyright=\"Canonical Ltd.\"
                            --package-name ubuntu-system-settings
                            --qt --c++ --add-comments=TRANSLATORS
                            --keyword=QT_TR_NOOP
                            --keyword=tr --keyword=tr:1,2 --from-code=UTF-8
                            ${QMLFILES} "${CMAKE_CURRENT_BINARY_DIR}/settings.js"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS ${QMLFILES} "${CMAKE_CURRENT_BINARY_DIR}/settings.js" #desktop.js
)

add_custom_target(pot-cpp
    COMMAND ${XGETTEXT_BIN} -o ubuntu-system-settings.pot
                            --join-existing
                            --copyright=\"Canonical Ltd.\"
                            --package-name ubuntu-system-settings
                            --qt --c++ --add-comments=TRANSLATORS
                            --keyword=_ --from-code=UTF-8
                            ${CPPFILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS pot-qml ${CPPFILES}
)

add_custom_target(pot-py
    COMMAND ${XGETTEXT_BIN} -o ubuntu-system-settings.pot
                            --join-existing
                            --copyright=\"Canonical Ltd.\"
                            --package-name ubuntu-system-settings
                            --add-comments=TRANSLATORS
                            --keyword=_
                            ${PYFILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS ${PYFILES}
)

add_custom_target(pot DEPENDS pot-qml pot-cpp pot-py)

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

