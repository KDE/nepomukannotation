project(scriboannotationplugin)

include(SopranoAddOntology)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}/nepomuk
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SCRIBO_INCLUDE_DIR}
  ${nepomukannotation_SOURCE_DIR}
)

set(scriboannotationplugin_SRC
  scriboannotationplugin.cpp
  occurrenceannotation.cpp
  eventannotation.cpp)

kde4_add_plugin(nepomuk_scriboannotationplugin ${scriboannotationplugin_SRC})
target_link_libraries(nepomuk_scriboannotationplugin
  ${NEPOMUK_LIBRARIES}
  ${NEPOMUK_QUERY_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${SCRIBO_LIBRARIES}
  ${NEPOMUKEXTRAS_LIBRARIES}
  nepomukannotation
)
install(TARGETS nepomuk_scriboannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES nepomuk_scriboannotationplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
