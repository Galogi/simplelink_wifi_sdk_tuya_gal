cmake_minimum_required(VERSION ${TI_MIN_CMAKE_VERSION})

cmake_path(GET CMAKE_CURRENT_LIST_DIR FILENAME    TARGET_NAME)


message(STATUS "Generating target           ${TARGET_NAME}")

#read the version file
file(READ lwip-stack/src/include/lwip/init.h lwip_versions)
#set the version
string(REGEX MATCH "LWIP_VERSION_MAJOR[ \t]+([0-9]+)" _ ${lwip_versions})
set(LWIP_VERSION_MAJOR       ${CMAKE_MATCH_1})
string(REGEX MATCH "LWIP_VERSION_MINOR[ \t]+([0-9]+)" _ ${lwip_versions})
set(LWIP_VERSION_MINOR       ${CMAKE_MATCH_1})
string(REGEX MATCH "LWIP_VERSION_REVISION[ \t]+([0-9]+)" _ ${lwip_versions})
set(LWIP_VERSION_REVISION       ${CMAKE_MATCH_1})
set_global(LWIP_VERSION       "${LWIP_VERSION_MAJOR}.${LWIP_VERSION_MINOR}.${LWIP_VERSION_REVISION}")

set(${TARGET_NAME}_SOURCES

    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/addons/ipv6_static_routing/ip6_route_table.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/addons/tcp_isn/tcp_isn.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/chargen/chargen.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/httpserver/httpserver-netconn.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/netio/netio.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/ping/ping.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/rtp/rtp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/shell/shell.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/socket_examples/socket_examples.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/tcpecho/tcpecho.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/tcpecho_raw/tcpecho_raw.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/udpecho/udpecho.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/apps/udpecho_raw/udpecho_raw.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/ethernetif/ethernetif.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/httpd/cgi_example/cgi_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/httpd/examples_fsdata.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/httpd/fs_example/fs_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/httpd/genfiles_example/genfiles_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/httpd/post_example/post_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/httpd/ssi_example/ssi_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/lwiperf/lwiperf_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/mdns/mdns_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/mqtt/mqtt_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/ppp/pppos_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/snmp/snmp_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/snmp/snmp_private_mib/lwip_prvmib.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/snmp/snmp_v3/snmpv3_dummy.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/sntp/sntp_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib/examples/tftp/tftp_example.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/api_lib.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/api_msg.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/err.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/if_api.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/netbuf.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/netdb.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/netifapi.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/sockets.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/api/tcpip.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/altcp_tls/altcp_tls_mbedtls.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/altcp_tls/altcp_tls_mbedtls_mem.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/http/altcp_proxyconnect.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/http/fs.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/http/fsdata.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/http/http_client.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/http/httpd.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/mdns/mdns.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/mqtt/mqtt.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/netbiosns/netbiosns.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/smtp/smtp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_asn1.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_core.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_icmp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_interfaces.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_ip.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_snmp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_system.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_tcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_mib2_udp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_msg.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_netconn.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_pbuf_stream.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_raw.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_scalar.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_snmpv2_framework.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_snmpv2_usm.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_table.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_threadsync.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmp_traps.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmpv3.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/snmp/snmpv3_mbedtls.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/sntp/sntp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/apps/tftp/tftp_server.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/altcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/altcp_alloc.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/altcp_tcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/def.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/dns.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/inet_chksum.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/init.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ip.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/mem.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/memp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/netif.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/pbuf.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/raw.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/stats.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/sys.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/tcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/tcp_in.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/tcp_out.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/timeouts.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/udp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/autoip.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/dhcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/etharp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/icmp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/igmp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/ip4.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/ip4_addr.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv4/ip4_frag.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/dhcp6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/ethip6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/icmp6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/inet6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/ip6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/ip6_addr.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/ip6_frag.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/mld6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/core/ipv6/nd6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/bridgeif.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/bridgeif_fdb.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ethernet.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/lowpan6.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/lowpan6_ble.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/lowpan6_common.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/slipif.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/zepif.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/auth.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/ccp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/chap-md5.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/chap-new.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/chap_ms.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/demand.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/eap.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/ecp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/eui64.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/fsm.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/ipcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/ipv6cp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/lcp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/magic.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/mppe.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/multilink.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/ppp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/pppapi.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/pppcrypt.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/pppoe.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/pppol2tp.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/pppos.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/upap.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/utils.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/vj.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/polarssl/arc4.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/polarssl/des.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/polarssl/md4.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/polarssl/md5.c
    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/netif/ppp/polarssl/sha1.c

    ${CMAKE_CURRENT_LIST_DIR}/ti_config/lwip-port/osi/sys_arch.c

)

set (${TARGET_NAME}_INCLUDES

    ${CMAKE_CURRENT_LIST_DIR}/lwip-stack/src/include
    ${CMAKE_CURRENT_LIST_DIR}/lwip-contrib
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/lwip-port/osi/include

    $<TARGET_PROPERTY:wifi_host_driver,INCLUDE_DIRECTORIES>

)

set (${TARGET_NAME}_LIBRARIES

    ${CMAKE_COMMON_LIBRARY}
)

set (${TARGET_NAME}_COMPILER_FLAGS -Wno-error -Ofast)

set (${TARGET_NAME}_DEFINITIONS )


add_library               (${TARGET_NAME}         ${${TARGET_NAME}_SOURCES})
target_compile_options    (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_COMPILER_FLAGS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_DEFINITIONS})
target_include_directories(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_INCLUDES})
target_precompile_headers (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_HEADERS})
target_link_libraries     (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_LIBRARIES})


#set install option
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DEST_DIR})
