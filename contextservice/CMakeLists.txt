include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${CMAKE_SOURCE_DIR}
  ${SOPRANO_INCLUDE_DIR}
  )

set(nepomukcontextservice_SRCS
  nepomukusercontextservice.cpp
  )

qt4_add_dbus_adaptor(nepomukcontextservice_SRCS
  org.kde.nepomuk.UserContextService.xml
  nepomukusercontextservice.h
  Nepomuk::UserContextService)

kde4_add_plugin(nepomukusercontextservice ${nepomukcontextservice_SRCS})

target_link_libraries(nepomukusercontextservice
  ${SOPRANO_CLIENT_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${NEPOMUK_LIBRARIES}
  )

install(
  FILES nepomukusercontextservice.desktop
  DESTINATION ${SERVICES_INSTALL_DIR})
install(
  TARGETS nepomukusercontextservice
  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES
  org.kde.nepomuk.UserContextService.xml
  DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}
  )
