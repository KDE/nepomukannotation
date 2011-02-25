kde4_add_plugin(nepomuk_pimoannotationplugin
  pimoannotationplugin.cpp
  ../common/pimoplugintools.cpp
)
target_link_libraries(nepomuk_pimoannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${NEPOMUK_QUERY_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
  nepomukextras
)
install(TARGETS nepomuk_pimoannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES pimoannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
