kde4_add_plugin(nepomuk_dbpediaannotationplugin
  dbpediaannotationplugin.cpp
  )

target_link_libraries(nepomuk_dbpediaannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${SOPRANO_CLIENT_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
)

install(TARGETS nepomuk_dbpediaannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES dbpediaannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
