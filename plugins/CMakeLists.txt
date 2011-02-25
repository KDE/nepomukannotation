include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${nepomukannotationplugins_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/common
)

find_package(Scribo)
macro_log_feature(SCRIBO_FOUND "Scribo" "Natural language processing for KDE" "scribo" FALSE "" "Required for natural language processing.")

add_subdirectory(pimo)
add_subdirectory(propertycreation)
add_subdirectory(pimotype)
add_subdirectory(tags)
add_subdirectory(webpage)
add_subdirectory(geonames)
add_subdirectory(dbpedia)
add_subdirectory(context)

if(SCRIBO_FOUND)
  add_subdirectory(scribo)
endif()

# NCO
#kde4_add_plugin(nepomuk_ncoannotationplugin nco.cpp)
#target_link_libraries(nepomuk_ncoannotationplugin
#  ${NEPOMUK_LIBRARIES}
#  ${SOPRANO_LIBRARIES}
#  ${KDE4_KDEUI_LIBS}
#  ${KDE4_KIO_LIBS}
#  nepomukpeopletag
#  nepomukannotation
#)
#install(TARGETS nepomuk_ncoannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
#install(FILES nco.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)

# Projects
#kde4_add_plugin(nepomuk_projectannotationplugin project.cpp)
#target_link_libraries(nepomuk_projectannotationplugin
#  ${NEPOMUK_LIBRARIES}
#  ${SOPRANO_LIBRARIES}
#  ${KDE4_KDEUI_LIBS}
#  ${KDE4_KIO_LIBS}
#  nepomukannotation
#)
#install(TARGETS nepomuk_projectannotationplugin DESTINATION ${PLUGIN_INSTALL_DIR})
#install(FILES project.desktop DESTINATION ${SERVICES_INSTALL_DIR}/nepomukannotationplugins)
