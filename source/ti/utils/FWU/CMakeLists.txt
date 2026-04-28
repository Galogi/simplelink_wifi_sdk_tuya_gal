cmake_minimum_required(VERSION ${TI_MIN_CMAKE_VERSION})

cmake_path(GET CMAKE_CURRENT_LIST_DIR FILENAME    TARGET_NAME)

set(TARGET_NAME ${PARENT_NAME} ${TARGET_NAME})


message(STATUS "Generating target           ${TARGET_NAME}")

set(${TARGET_NAME}_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/psa_fwu.c
)

# Add the upper_mac_versions.h directory to the include_directories
include_directories(${UPPER_MAC_VERSION_DIR})

set(${TARGET_NAME}_INCLUDES

    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/headers/

    #external includes
    ${CORESDK_WFF3}/source
    ${CORESDK_WFF3}/source/ti/drivers/xmem/

    $<TARGET_PROPERTY:wifi_host_driver,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:wifi_platform_cc35xx,INCLUDE_DIRECTORIES>
)

set (${TARGET_NAME}_LIBRARIES

    ${CMAKE_COMMON_LIBRARY}

)

set (${TARGET_NAME}_COMPILER_FLAGS )

set (${TARGET_NAME}_DEFINITIONS )

set (${TARGET_NAME}_HEADERS )


add_library               (${TARGET_NAME}         ${${TARGET_NAME}_SOURCES})
target_compile_options    (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_COMPILER_FLAGS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_DEFINITIONS})
target_include_directories(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_INCLUDES})
target_precompile_headers (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_HEADERS})
target_link_libraries     (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_LIBRARIES})

#set install option
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DEST_DIR})
