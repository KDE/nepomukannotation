include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}/nepomuk
  ${CMAKE_SOURCE_DIR}
  ${nepomukannotationplugins_SOURCE_DIR}
  ${nepomukannotation_SOURCE_DIR}/gui
  ${STRIGI_INCLUDE_DIR}
)

set(nepomuktextannotator_SRCS
  mainwindow.cpp
  resourceannotater.cpp
  textannotationwidget.cpp
)

kde4_add_ui_files(nepomuktextannotator_SRCS mainwindow.ui)

kde4_add_executable(nepomuktextannotator ${nepomuktextannotator_SRCS})

target_link_libraries(nepomuktextannotator
  nepomukannotation
  ${NEPOMUK_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${STRIGI_STREAMANALYZER_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)

install(TARGETS nepomuktextannotator DESTINATION ${BIN_INSTALL_DIR})
install(FILES nepomuktextannotator-app.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
