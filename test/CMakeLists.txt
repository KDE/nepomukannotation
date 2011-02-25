include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}/nepomuk
  ${CMAKE_SOURCE_DIR}
  ${nepomukannotationplugins_SOURCE_DIR}
  ${nepomukannotation_SOURCE_DIR}/gui
  ${nepomukextras_SOURCE_DIR}
)

kde4_add_executable(statementeditortest statementeditortest.cpp)
target_link_libraries(statementeditortest
  nepomukextras
  nepomukannotation
  ${NEPOMUK_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)


kde4_add_executable(annotationmenutester annotationmenutester.cpp)
target_link_libraries(annotationmenutester
  nepomukextras
  nepomukannotation
  ${NEPOMUK_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
)
