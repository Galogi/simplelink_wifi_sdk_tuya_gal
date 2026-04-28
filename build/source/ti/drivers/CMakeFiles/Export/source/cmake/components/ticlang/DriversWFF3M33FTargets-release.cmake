#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Drivers::drivers_cc35xx" for configuration "Release"
set_property(TARGET Drivers::drivers_cc35xx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Drivers::drivers_cc35xx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/source/ti/drivers/lib/ticlang/m33f/drivers_cc35xx.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Drivers::drivers_cc35xx )
list(APPEND _IMPORT_CHECK_FILES_FOR_Drivers::drivers_cc35xx "${_IMPORT_PREFIX}/source/ti/drivers/lib/ticlang/m33f/drivers_cc35xx.a" )

# Import target "Drivers::drivers_cc35xx_log" for configuration "Release"
set_property(TARGET Drivers::drivers_cc35xx_log APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Drivers::drivers_cc35xx_log PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/source/ti/drivers/lib/ticlang/m33f/drivers_cc35xx_log.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Drivers::drivers_cc35xx_log )
list(APPEND _IMPORT_CHECK_FILES_FOR_Drivers::drivers_cc35xx_log "${_IMPORT_PREFIX}/source/ti/drivers/lib/ticlang/m33f/drivers_cc35xx_log.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
