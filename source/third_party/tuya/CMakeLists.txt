cmake_minimum_required(VERSION 3.21)
project(tuya)
set(TARGET_NAME tuya)
message(STATUS "Generating target ${TARGET_NAME}")

# --- 1. DEFINE SOURCES ---

set(${TARGET_NAME}_SOURCES
    # Tuya OS Adapter (TKL)
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_adc.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_asymmetrical.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_bluetooth_bredr.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_bluetooth_mesh_device.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_bluetooth_mesh_prov.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_bluetooth_sm.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_bluetooth.c

    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_board.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_flash.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_fs.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_gpio.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_hash.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_i2c.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_i2s.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_lwip.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_memory.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_mutex.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_network.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_ota.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_output.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_pwm.c

    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_rtc.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_sleep.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_spi.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_symmetry.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_system.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_timer.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_uart.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_wakeup.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_watchdog.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_wired.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_wifi.c

    ${CMAKE_CURRENT_LIST_DIR}/ti_config/wrapper_config/wrapper_config.c


    # Initialization
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_system.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_network.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_wifi.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_common.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_wired.c
     ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/utilities/src/tuya_list.c
     ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/utilities/src/tuya_queue.c
  

    # Utilities & JSON Core
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities/crc32i.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities/crc_16.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities/mix_method.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities/uni_random.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/backoffAlgorithm/source/backoff_algorithm.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libcjson/cJSON/cJSON.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/src/http_client_wrapper.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/src/http_download.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/src/cipher_wrapper.c

    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_kv/src/tal_kv.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_kv/src/kv_serialize.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_kv/littlefs/lfs.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_security/src/tal_symmetry.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_security/src/tal_hash.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_network/src/tal_network_register.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_network/src/tal_platform.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/coreHTTP/source/core_http_client.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libmqtt/src/mqtt_client_wrapper.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libmqtt/coreMQTT/source/core_mqtt.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libmqtt/coreMQTT/source/core_mqtt_serializer.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libmqtt/coreMQTT/source/core_mqtt_state.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.c
 

    # TAL
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_api.c

    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_event.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_fs.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_log.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_sleep.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_sw_timer.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_system.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_time_serivce.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_workq_service.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_workqueue.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_wifi/src/tal_wifi.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_network/src/tal_network.c
    

    # Cloud Services
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_iot.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/atop_base.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/atop_service.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/iotdns.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/matop_service.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/mqtt_bind.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_endpoint.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_health.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_http.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_ota.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_register_center.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/mqtt_service.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/protocol/tuya_protocol.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/lan/lan_sock.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/lan/tuya_lan.c
      
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/schema/dp_schema.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/schema/tuya_iot_dp.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/tls/tuya_tls.c
     ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netmgr/netmgr.c
     ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netcfg/netcfg.c
     ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netcfg/ap_pbkdf2.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netcfg/ap_netcfg.c
     ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netmgr/netconn_wifi.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/transport/tuya_transport.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/transport/tcp_transporter.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/transport/tls_transporter.c
)
    set(TI_SYSCONFIG_TOOL "C:/ti/sysconfig_1.26.0/sysconfig_cli.bat")
# --- 2. DEFINE INCLUDES ---

set(${TARGET_NAME}_INCLUDES
    # 1. Compiler Toolchain Includes
    "C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/include/c"
    

    # 2. Custom and Local Configs
    ${CMAKE_CURRENT_LIST_DIR}/ti_config

    # 3. Tuya Core SDK Headers
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/common
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/libmqtt
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/libhttp
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/libutils
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/coreHTTP
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/coreHTTP/source/include
    
   

    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libhttp/coreHTTP/source/dependency/3rdparty/http_parser

    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_network/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_wifi/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_wired/include
   

    # 4. FreeRTOS Core & Config
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/freertos/include"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/freertos/portable/GCC/ARM_CM33_NTZ/non_secure"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/kernel/freertos/builds/.meta/cc35xx/release/pregenerated_configuration"
    
    # 5. TI SDK Base
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source"

    # 6. Cloud Service Subfolders
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netmgr
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/schema
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/protocol
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/tls
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/lan
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/transport
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netcfg
    

    # 7. Utilities, Security & Storage
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/port
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/include
    C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/mbedtls/include
    C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/mbedtls/ti/configs
    C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/mbedtls/ti/port
    C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/mbedtls/ti
    
    ${CMAKE_CURRENT_LIST_DIR}/third_party/hostap/src/openssl/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_kv/littlefs
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libcjson/cJSON
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_kv/port

    # 8. TAL Subfolders & Utilities
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_cli
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_bluetooth
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_cellular
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_driver
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_kv
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_security
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/backoffAlgorithm/source/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libmqtt/coreMQTT/source/include
    

    # 9. TI Adaptation Headers (TKL)
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/system
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/wifi
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/network
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/uart
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/adc
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/bluetooth
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/flash
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/gpio
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/i2c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/i2s
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/pwm
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/rtc
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/security
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/spi
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/wakeup
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/watchdog
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/timer
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/wired
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/utilities/include

    # 10. Explicit TI SDK Base & LwIP Includes
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/ti/drivers/net/wifi"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/kernel/freertos/posix"

    # --- LwIP Stack Headers ---
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/lwip/lwip-stack/src/include"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/ti/net/lwip"
    
    # --- ADDED: LwIP Config/Opts for FreeRTOS (Fixes lwipopts.h error) ---
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/lwip/ti_config/lwip-port/freertos/include"
    # ---------------------------------------------------------------------

    # 11. Property-based Includes
    $<TARGET_PROPERTY:wifi_host_driver,INCLUDE_DIRECTORIES>
)

# --- 3. COMPILER SETTINGS ---

set(${TARGET_NAME}_LIBRARIES ${CMAKE_COMMON_LIBRARY})

set(${TARGET_NAME}_DEFINITIONS 
    -D_TUYA_TI_PLATFORM_
    -DTUYA_GW_OPERATING_SYSTEM=1
    -DDeviceFamily_CC35XX
)
list(APPEND ${TARGET_NAME}_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/aes.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/ccm.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/cipher.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/cipher_wrap.c
    
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/ctr_drbg.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/entropy.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/entropy_poll.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/gcm.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/aria.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/camellia.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/des.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/nist_kw.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/ripemd160.c
    
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/md5.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/platform.c
    
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/sha1.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/sha256.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/library/sha512.c
)

set(${TARGET_NAME}_COMPILER_FLAGS -Ofast -Wno-error)

# --- 4. CREATE LIBRARY ---

add_library(${TARGET_NAME} STATIC ${${TARGET_NAME}_SOURCES})

target_compile_options(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_COMPILER_FLAGS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_DEFINITIONS})
target_compile_definitions(${TARGET_NAME} PRIVATE
    MBEDTLS_CONFIG_FILE="mbedtls/mbedtls_config.h"
)
target_compile_definitions(${TARGET_NAME} PRIVATE
    EOF=-1
)
target_compile_definitions(${TARGET_NAME} PRIVATE TUYA_SECURITY_LEVEL=0)
target_compile_definitions(${TARGET_NAME} PRIVATE LFS_CONFIG=lfs_config.h)
target_compile_definitions(${TARGET_NAME} PRIVATE
    OPERATING_SYSTEM=SYSTEM_FREERTOS
)
target_compile_definitions(${TARGET_NAME} PRIVATE
    PJ_HAS_SYS_TYPES_H=0
    PJ_HAS_SYS_SOCKET_H=0
    PJ_HAS_SYS_IOCTL_H=0
    PJ_HAS_NET_IF_H=0
    PJ_HAS_ARPA_INET_H=0
    PJ_HAS_UNISTD_H=0
    PJ_HAS_PTHREAD_H=0
    PJ_HAS_ERRNO_H=0
)

target_include_directories(${TARGET_NAME} PUBLIC ${${TARGET_NAME}_INCLUDES})
target_link_libraries(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DEST_DIR})
