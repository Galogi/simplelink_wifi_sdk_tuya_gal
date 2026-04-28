#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Driverlib::cc35xx" for configuration "Release"
set_property(TARGET Driverlib::cc35xx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Driverlib::cc35xx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/source/ti/devices/cc35xx/driverlib/lib/ticlang/driverlib.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Driverlib::cc35xx )
list(APPEND _IMPORT_CHECK_FILES_FOR_Driverlib::cc35xx "${_IMPORT_PREFIX}/source/ti/devices/cc35xx/driverlib/lib/ticlang/driverlib.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
