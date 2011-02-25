kde4_add_plugin(nepomuk_pimotypeannotationplugin
  pimotypeannotationplugin.cpp
  pimotypeannotation.cpp
  ../common/pimoplugintools.cpp
)
target_link_libraries(nepomuk_pimotypeannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
  nepomukextras
)
install(TARGETS nepomuk_pimotypeannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES pimotypeannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
