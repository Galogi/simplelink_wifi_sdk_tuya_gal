#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SecureDrivers::secure_drivers_cc35xx" for configuration "Release"
set_property(TARGET SecureDrivers::secure_drivers_cc35xx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SecureDrivers::secure_drivers_cc35xx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/source/ti/drivers/secure/lib/gcc/m33f/secure_drivers_cc35xx.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS SecureDrivers::secure_drivers_cc35xx )
list(APPEND _IMPORT_CHECK_FILES_FOR_SecureDrivers::secure_drivers_cc35xx "${_IMPORT_PREFIX}/source/ti/drivers/secure/lib/gcc/m33f/secure_drivers_cc35xx.a" )

# Import target "SecureDrivers::secure_drivers_cc35xx_mbedtls" for configuration "Release"
set_property(TARGET SecureDrivers::secure_drivers_cc35xx_mbedtls APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SecureDrivers::secure_drivers_cc35xx_mbedtls PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/source/ti/drivers/secure/lib/gcc/m33f/secure_drivers_cc35xx_mbedtls.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS SecureDrivers::secure_drivers_cc35xx_mbedtls )
list(APPEND _IMPORT_CHECK_FILES_FOR_SecureDrivers::secure_drivers_cc35xx_mbedtls "${_IMPORT_PREFIX}/source/ti/drivers/secure/lib/gcc/m33f/secure_drivers_cc35xx_mbedtls.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
