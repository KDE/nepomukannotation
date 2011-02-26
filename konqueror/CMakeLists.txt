project(konqui_menuplugin)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${KDE4WORKSPACE_INCLUDE_DIR}
  ${SOPRANO_INCLUDES}
)

set(nepomukmenuplugin_SRCS
  nepomukmenuplugin.cpp
)

kde4_add_plugin(nepomukmenuplugin ${nepomukmenuplugin_SRCS})

target_link_libraries(nepomukmenuplugin
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KHTML_LIBS}
  ${NEPOMUKEXTRAS_LIBRARIES}
  konq
  nepomukannotation
)

install(TARGETS nepomukmenuplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES nepomukmenuplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})

