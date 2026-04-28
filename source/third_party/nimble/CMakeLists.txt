cmake_minimum_required(VERSION ${TI_MIN_CMAKE_VERSION})

cmake_path(GET CMAKE_CURRENT_LIST_DIR FILENAME    TARGET_NAME)


message(STATUS "Generating target           ${TARGET_NAME}")

#read the version file
file(READ nimble-src/RELEASE_NOTES.md nimble_versions)
#set the version
string(REGEX MATCH "Apache NimBLE[ \t]+v([0-9|.]+)" _ ${nimble_versions})
set_global(NIMBLE_VERSION       ${CMAKE_MATCH_1})

set(${TARGET_NAME}_SOURCES

    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/aes_decrypt.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/aes_encrypt.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/cbc_mode.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ccm_mode.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/cmac_mode.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ctr_mode.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ctr_prng.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ecc.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ecc_dh.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ecc_dsa.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/ecc_platform_specific.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/hmac.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/hmac_prng.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/sha256.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/src/utils.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/ans/src/ble_svc_ans.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/bas/src/ble_svc_bas.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/dis/src/ble_svc_dis.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/gap/src/ble_svc_gap.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/gatt/src/ble_svc_gatt.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/ias/src/ble_svc_ias.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/ipss/src/ble_svc_ipss.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/lls/src/ble_svc_lls.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/tps/src/ble_svc_tps.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_att.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_att_clt.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_att_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_att_svr.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_dtm.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_eatt.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_eddystone.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_gap.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_gattc.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_gatts.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_gatts_lcl.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_adv.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_atomic.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_cfg.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_conn.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_flow.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_hci.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_hci_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_hci_evt.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_hci_util.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_id.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_log.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_mbuf.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_misc.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_mqueue.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_periodic_sync.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_pvcy.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_shutdown.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_startup.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_hs_stop.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_ibeacon.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_l2cap.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_l2cap_coc.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_l2cap_sig.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_l2cap_sig_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_sm.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_sm_alg.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_sm_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_sm_lgcy.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_sm_sc.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_store.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_store_util.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/src/ble_uuid.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/store/config/src/ble_store_config.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/store/ram/src/ble_store_ram.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/util/src/addr.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/common/hci_h4/src/hci_h4.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/common/hci_ipc/src/hci_ipc.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/src/monitor.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/src/transport.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/endian.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/mem.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/nimble_port.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/os_cputime.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/os_cputime_pwr2.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/os_mbuf.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/os_mempool.c
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/src/os_msys_init.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/porting/nimble/src/base64.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/porting/npl/osi/src/npl_os_osi.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/porting/npl/osi/src/nimble_osi_filesystem.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/store/config/src/ble_store_config_conf.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/transport/cc3xxxhif/src/cc3xxxhif_ble_hci.c

)

set (${TARGET_NAME}_INCLUDES

    #local includes
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/include/console
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/include/hal
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/include/syscfg
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/porting/nimble/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/porting/npl/osi/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/nimble-port/transport/cc3xxxhif/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/ext/tinycrypt/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/ans/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/bas/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/bleuart/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/dis/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/gap/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/gatt/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/ias/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/ipss/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/lls/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/services/tps/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/store/config/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/store/ram/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/host/util/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/common/hci_h4/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/common/hci_ipc/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/nimble/transport/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/porting/nimble/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble-src/transport/cc3xxxhif/include
    #external includes
    ${WIFI_HOST_INTERFACE}/inc_adapt

    $<TARGET_PROPERTY:ble_interface,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:wifi_host_driver,INCLUDE_DIRECTORIES>

)

set (${TARGET_NAME}_LIBRARIES

    ${CMAKE_COMMON_LIBRARY}
)

set (${TARGET_NAME}_COMPILER_FLAGS )

set (${TARGET_NAME}_DEFINITIONS )


add_library               (${TARGET_NAME}         ${${TARGET_NAME}_SOURCES})
target_compile_options    (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_COMPILER_FLAGS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_DEFINITIONS})
target_include_directories(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_INCLUDES})
target_precompile_headers (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_HEADERS})
target_link_libraries     (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_LIBRARIES})


#set install option
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DEST_DIR})
