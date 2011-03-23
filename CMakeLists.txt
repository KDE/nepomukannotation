project(nepomukannotation)

# set the version to be used by NepomukExtrasConfig.cmake and the lib version
# ===============================================================================================
set(CMAKE_NEPOMUKANNOTATION_VERSION_MAJOR 0)
set(CMAKE_NEPOMUKANNOTATION_VERSION_MINOR 2)
set(CMAKE_NEPOMUKANNOTATION_VERSION_RELEASE 60)
set(CMAKE_NEPOMUKANNOTATION_VERSION "${CMAKE_NEPOMUKANNOTATION_VERSION_MAJOR}.${CMAKE_NEPOMUKANNOTATION_VERSION_MINOR}.${CMAKE_NEPOMUKANNOTATION_VERSION_RELEASE}")


# search required packages
# ===============================================================================================
set(KDE_MIN_VERSION "4.5.00")
find_package(KDE4 4.5.00 REQUIRED)
include(KDE4Defaults)
find_package(KDE4Workspace)
include(MacroLibrary)

find_package(SharedDesktopOntologies 0.5)
set(SOPRANO_MIN_VERSION "2.5.0")
find_package(Soprano)
find_package(Nepomuk)
find_package(NepomukExtras)

macro_log_feature(SOPRANO_FOUND "Soprano" "Support for the Nepomuk semantic desktop system" "http://soprano.sourceforge.net" TRUE "${SOPRANO_MIN_VERSION}" "")
macro_log_feature(SHAREDDESKTOPONTOLOGIES_FOUND "Shared desktop ontologies" "Desktop ontologies" "http://oscaf.sourceforge.net" TRUE ${SDO_MIN_VERSION} "Ontologies necessary for the Nepomuk semantic desktop.")
macro_log_feature(Nepomuk_FOUND "Nepomuk" "Nepomuk Libraries" "kdelibs" TRUE "" "The basis for this package.")
macro_log_feature(NepomukExtras_FOUND "Nepomukextras" "Nepomuk extras - a set of convinient classes" "nepomukextras" TRUE "" "Required for text extraction.")

# generate NepomukAnnotationConfig.cmake for easy utilisation of the package by other cmake build systems
# ===============================================================================================
configure_file(NepomukAnnotationConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/NepomukAnnotationConfig.cmake @ONLY)
configure_file(NepomukAnnotationConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/NepomukAnnotationConfigVersion.cmake @ONLY)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/NepomukAnnotationConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/NepomukAnnotationConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/NepomukAnnotation)


# build stuff
# ===============================================================================================
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DDISABLE_NEPOMUK_LEGACY)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUKEXTRAS_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/gui
  ${CMAKE_CURRENT_BINARY_DIR}/lib
)

add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(simpleannotator)
add_subdirectory(textannotator)
add_subdirectory(resourceeditor)
add_subdirectory(test)
add_subdirectory(contextservice)
add_subdirectory(contextplasmoid)
add_subdirectory(konqueror)
add_subdirectory(fileitemplugin)

macro_display_feature_log()
