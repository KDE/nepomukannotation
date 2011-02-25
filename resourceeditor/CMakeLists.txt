kde4_add_executable(resourceeditor main.cpp)
target_link_libraries(resourceeditor
  nepomukextras
  nepomukannotation
  ${NEPOMUK_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)

install(TARGETS resourceeditor DESTINATION ${BIN_INSTALL_DIR})
#install(FILES resourceeditor.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
