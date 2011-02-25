include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(geonames_plugin_SRCS
  geonamesannotationplugin.cpp
  graphretriever.cpp
  ../common/pimoplugintools.cpp
)

kde4_add_plugin(nepomuk_geonamesannotationplugin
  ${geonames_plugin_SRCS}
  )

target_link_libraries(nepomuk_geonamesannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
  nepomukextras
)

install(TARGETS nepomuk_geonamesannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES geonamesannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
