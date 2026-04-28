#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Device::cc35xx_startup" for configuration "Release"
set_property(TARGET Device::cc35xx_startup APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Device::cc35xx_startup PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/source/ti/devices/cc35xx/startup_files/lib/gcc/device_cc35xx_startup.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Device::cc35xx_startup )
list(APPEND _IMPORT_CHECK_FILES_FOR_Device::cc35xx_startup "${_IMPORT_PREFIX}/source/ti/devices/cc35xx/startup_files/lib/gcc/device_cc35xx_startup.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
