include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}/nepomuk
  ${CMAKE_SOURCE_DIR}
  ${nepomukannotationplugins_SOURCE_DIR}
  ${nepomukannotation_SOURCE_DIR}/gui
)

set(nepomuksimpleannotator_SRCS
  mainwindow.cpp
  resourceannotater.cpp
)

kde4_add_executable(nepomuksimpleannotator ${nepomuksimpleannotator_SRCS})

target_link_libraries(nepomuksimpleannotator
  nepomukannotation
  ${NEPOMUK_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)

install(TARGETS nepomuksimpleannotator DESTINATION ${BIN_INSTALL_DIR})
install(FILES nepomuksimpleannotator.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
