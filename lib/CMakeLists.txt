project(libnepomukannotation)

include(SopranoAddOntology)

configure_file(nepomukannotation_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/nepomukannotation_version.h)

set( corelib_SRCS
    annotationpluginfactory.cpp
    annotationplugin.cpp
    annotationpluginwrapper.cpp
    annotation.cpp
    annotationrequest.cpp
    tagannotation.cpp
    resourceactionplugin.cpp
    simpleannotation.cpp
    simplepimoannotation.cpp
    textannotation.cpp
    stackedtextannotation.cpp
    stackedpimotextannotation.cpp
    nepomukcontext.cpp
    dbusconnectionpool.cpp
    nepomukannotation_version.cpp
    gui/annotationrelevancefiltermodel.cpp
    gui/resourceannotationmodel.cpp
    gui/annotationmodel.cpp
    gui/annotationwidget.cpp
    gui/annotationdisplay.cpp
    gui/annotationdisplaybrowser.cpp
    gui/annotationdelegate.cpp
    gui/thingmodel.cpp
    gui/thingview.cpp
    gui/thingitemdelegate.cpp
    gui/relationmodel.cpp
    gui/relationview.cpp
    gui/relationitemdelegate.cpp
    gui/resourcerelationview.cpp
    gui/resourceeditor.cpp
    gui/resourceeditorannotationwidget.cpp
    gui/labeleditwidget.cpp
    gui/annotationmenu.cpp
    gui/kedittagsdialog.cpp
)

qt4_add_dbus_interface(corelib_SRCS "../contextservice/org.kde.nepomuk.UserContextService.xml" nepomukcontextserviceinterface)

soprano_add_ontology(corelib_SRCS ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nepomuk/nuao.trig "NUAO" "Nepomuk::Vocabulary" "trig")
soprano_add_ontology(corelib_SRCS ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/pimo/tmo.trig "TMO" "Nepomuk::Vocabulary" "trig")

kde4_add_ui_files(corelib_SRCS gui/resourceeditor.ui)

kde4_add_library(nepomukannotation SHARED ${corelib_SRCS})

target_link_libraries(
  nepomukannotation 
  ${NEPOMUK_LIBRARIES}
  ${NEPOMUK_QUERY_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${SOPRANO_INDEX_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUKEXTRAS_LIBRARIES}
)

set_target_properties(nepomukannotation PROPERTIES VERSION ${CMAKE_NEPOMUKANNOTATION_VERSION} SOVERSION ${CMAKE_NEPOMUKANNOTATION_VERSION_MAJOR})

install(TARGETS nepomukannotation DESTINATION ${LIB_INSTALL_DIR})
install(FILES
  nepomukannotation_export.h
  annotationpluginfactory.h
  annotationpluginwrapper.h
  annotationplugin.h
  annotation.h
  annotationrequest.h
  tagannotation.h
  simpleannotation.h
  simplepimoannotation.h
  stackedtextannotation.h
  stackedpimotextannotation.h
  textannotation.h
  nepomukcontext.h
  gui/annotationrelevancefiltermodel.h
  gui/annotationmodel.h
  gui/resourceannotationmodel.h
  gui/annotationwidget.h
  gui/annotationdisplay.h
  gui/annotationdelegate.h
  gui/thingmodel.h
  gui/thingview.h
  gui/relationmodel.h
  gui/relationview.h
  gui/resourcerelationview.h
  gui/resourceeditor.h
  gui/annotationmenu.h
  ${CMAKE_CURRENT_BINARY_DIR}/nepomukannotation_version.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/nepomuk)
install(FILES nepomuk-annotationplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "${QT_DOC_DIR}/html")
  else(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "http://doc.trolltech.com/4.5/")
  endif(EXISTS ${QT_DOC_DIR}/html)

  add_custom_target(
    apidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND docs/html/installdox -l qt4.tag@${QTDOCS} docs/html/*.html)
endif(DOXYGEN_EXECUTABLE)
