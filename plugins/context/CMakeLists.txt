include_directories(
  ${nepomuk_kde_SOURCE_DIR}/usercontext/lib
  ${nepomuk_kde_BINARY_DIR}/usercontext/lib
)

kde4_add_plugin(nepomuk_contextannotationplugin
  contextannotationplugin.cpp
  )

target_link_libraries(nepomuk_contextannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${SOPRANO_CLIENT_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  nepomukannotation
  nepomukcontext
  nepomukextras
)

install(TARGETS nepomuk_contextannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES contextannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
