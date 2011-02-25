# Project Needs a name ofcourse
project(plasma-nepomukcontextchooser)
 
include_directories(
   ${CMAKE_CURRENT_BINARY_DIR}
   )
 
# We add our source code here
set(nepomukcontextchooser_SRCS 
  nepomukcontextchooser.cpp
  configwidget.cpp)

kde4_add_ui_files(nepomukcontextchooser_SRCS configwidget.ui)

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_nepomukcontextchooser ${nepomukcontextchooser_SRCS})
target_link_libraries(plasma_applet_nepomukcontextchooser 
  nepomukannotation
  ${NEPOMUKEXTRAS_LIBRARIES}
  ${KDE4_PLASMA_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
)
 
install(TARGETS plasma_applet_nepomukcontextchooser
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-nepomukcontextchooser.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
