project(fileitemplugin)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${KDE4WORKSPACE_INCLUDE_DIR}
  ${SOPRANO_INCLUDES}
)

set(nepomukfileitemplugin_SRCS
  nepomukfileitemplugin.cpp
)

kde4_add_plugin(nepomukfileitemplugin ${nepomukfileitemplugin_SRCS})

target_link_libraries(nepomukfileitemplugin
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${NEPOMUKEXTRAS_LIBRARIES}
  nepomukannotation
)

install(TARGETS nepomukfileitemplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES nepomukfileitemplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})

