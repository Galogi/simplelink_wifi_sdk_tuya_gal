# Install script for directory: C:/ti/Tuya/simplelink_wifi_sdk_tuya/source/ti/devices/cc35xx

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/ti/Tuya/simplelink_wifi_sdk_tuya")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "CMAKE_OBJDUMP-NOTFOUND")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/ti/devices/cc35xx/driverlib/lib/ticlang" TYPE STATIC_LIBRARY FILES "C:/ti/Tuya/simplelink_wifi_sdk_tuya/build/source/ti/devices/cc35xx/lib/ticlang/m33f/driverlib/driverlib.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets.cmake"
         "C:/ti/Tuya/simplelink_wifi_sdk_tuya/build/source/ti/devices/cc35xx/CMakeFiles/Export/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/cmake/components/ticlang" TYPE FILE FILES "C:/ti/Tuya/simplelink_wifi_sdk_tuya/build/source/ti/devices/cc35xx/CMakeFiles/Export/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/cmake/components/ticlang" TYPE FILE FILES "C:/ti/Tuya/simplelink_wifi_sdk_tuya/build/source/ti/devices/cc35xx/CMakeFiles/Export/source/cmake/components/ticlang/DriverlibCC35XXM33FTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/ti/Tuya/simplelink_wifi_sdk_tuya/source/cmake/components/ticlang/DriverlibCC35XXConfig.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/ti/Tuya/simplelink_wifi_sdk_tuya/source/cmake/components/ticlang" TYPE FILE FILES "C:/ti/Tuya/simplelink_wifi_sdk_tuya/build/source/ti/devices/cc35xx/source/cmake/components/ticlang/DriverlibCC35XXConfig.cmake")
endif()

