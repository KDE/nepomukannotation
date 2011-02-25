set(webpage_SRC webpage.cpp)

kde4_add_plugin(nepomuk_webpageannotationplugin ${webpage_SRC})
target_link_libraries(nepomuk_webpageannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
  nepomukextras
)
install(TARGETS nepomuk_webpageannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES nepomuk_webpageannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
