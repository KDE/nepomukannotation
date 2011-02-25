kde4_add_plugin(nepomuk_tagsannotationplugin
  tags.cpp
  ../common/pimoplugintools.cpp)

target_link_libraries(nepomuk_tagsannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
)

install(TARGETS nepomuk_tagsannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES nepomuk_tagsannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
