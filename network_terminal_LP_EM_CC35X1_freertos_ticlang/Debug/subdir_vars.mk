################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../linker.cmd 

SYSCFG_SRCS += \
../network_terminal.syscfg 

C_SRCS += \
../ble_cmd.c \
../calibrator.c \
../cmd_parser.c \
../crc.c \
../date_time_service.c \
../dhcpserver.c \
../lwip_iperf_examples.c \
../lwip_iperf_tcp_client.c \
../lwip_iperf_tcp_server.c \
../lwip_iperf_udp_client.c \
../lwip_iperf_udp_server.c \
../lwip_ping.c \
../main_freertos.c \
../network_lwip.c \
../network_mbedtls.c \
../network_terminal.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_freertos_config.c \
./syscfg/ti_freertos_portable_config.c \
./syscfg/ti_flash_map_config.c \
../nimble_host.c \
../nvocmp_cc35xx.c \
../sntp_task.c \
../sntp_wrapper.c \
../socket_examples.c \
../str.c \
../wlan_cmd.c 

GEN_FILES += \
./syscfg/ti_drivers_config.c \
./syscfg/ti_utils_build_compiler.opt \
./syscfg/ti_freertos_config.c \
./syscfg/ti_freertos_portable_config.c \
./syscfg/ti_flash_map_config.c 

GEN_MISC_DIRS += \
./syscfg 

C_DEPS += \
./ble_cmd.d \
./calibrator.d \
./cmd_parser.d \
./crc.d \
./date_time_service.d \
./dhcpserver.d \
./lwip_iperf_examples.d \
./lwip_iperf_tcp_client.d \
./lwip_iperf_tcp_server.d \
./lwip_iperf_udp_client.d \
./lwip_iperf_udp_server.d \
./lwip_ping.d \
./main_freertos.d \
./network_lwip.d \
./network_mbedtls.d \
./network_terminal.d \
./syscfg/ti_drivers_config.d \
./syscfg/ti_freertos_config.d \
./syscfg/ti_freertos_portable_config.d \
./syscfg/ti_flash_map_config.d \
./nimble_host.d \
./nvocmp_cc35xx.d \
./sntp_task.d \
./sntp_wrapper.d \
./socket_examples.d \
./str.d \
./wlan_cmd.d 

GEN_OPTS += \
./syscfg/ti_utils_build_compiler.opt 

OBJS += \
./ble_cmd.o \
./calibrator.o \
./cmd_parser.o \
./crc.o \
./date_time_service.o \
./dhcpserver.o \
./lwip_iperf_examples.o \
./lwip_iperf_tcp_client.o \
./lwip_iperf_tcp_server.o \
./lwip_iperf_udp_client.o \
./lwip_iperf_udp_server.o \
./lwip_ping.o \
./main_freertos.o \
./network_lwip.o \
./network_mbedtls.o \
./network_terminal.o \
./syscfg/ti_drivers_config.o \
./syscfg/ti_freertos_config.o \
./syscfg/ti_freertos_portable_config.o \
./syscfg/ti_flash_map_config.o \
./nimble_host.o \
./nvocmp_cc35xx.o \
./sntp_task.o \
./sntp_wrapper.o \
./socket_examples.o \
./str.o \
./wlan_cmd.o 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/ti_utils_build_linker.cmd.genmap \
./syscfg/syscfg_c.rov.xs \
./syscfg/FreeRTOSConfig.h \
./syscfg/external_memory_configurator.json \
./syscfg/ti_build_linker.cmd.toolbox \
./syscfg/ti_flash_map_config.h \
./syscfg/action_params.json \
./syscfg/action_request_extra.txt \
./syscfg/cc35xx-conf.ini 

GEN_MISC_DIRS__QUOTED += \
"syscfg" 

OBJS__QUOTED += \
"ble_cmd.o" \
"calibrator.o" \
"cmd_parser.o" \
"crc.o" \
"date_time_service.o" \
"dhcpserver.o" \
"lwip_iperf_examples.o" \
"lwip_iperf_tcp_client.o" \
"lwip_iperf_tcp_server.o" \
"lwip_iperf_udp_client.o" \
"lwip_iperf_udp_server.o" \
"lwip_ping.o" \
"main_freertos.o" \
"network_lwip.o" \
"network_mbedtls.o" \
"network_terminal.o" \
"syscfg\ti_drivers_config.o" \
"syscfg\ti_freertos_config.o" \
"syscfg\ti_freertos_portable_config.o" \
"syscfg\ti_flash_map_config.o" \
"nimble_host.o" \
"nvocmp_cc35xx.o" \
"sntp_task.o" \
"sntp_wrapper.o" \
"socket_examples.o" \
"str.o" \
"wlan_cmd.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.genlibs" \
"syscfg\ti_utils_build_linker.cmd.genmap" \
"syscfg\syscfg_c.rov.xs" \
"syscfg\FreeRTOSConfig.h" \
"syscfg\external_memory_configurator.json" \
"syscfg\ti_build_linker.cmd.toolbox" \
"syscfg\ti_flash_map_config.h" \
"syscfg\action_params.json" \
"syscfg\action_request_extra.txt" \
"syscfg\cc35xx-conf.ini" 

C_DEPS__QUOTED += \
"ble_cmd.d" \
"calibrator.d" \
"cmd_parser.d" \
"crc.d" \
"date_time_service.d" \
"dhcpserver.d" \
"lwip_iperf_examples.d" \
"lwip_iperf_tcp_client.d" \
"lwip_iperf_tcp_server.d" \
"lwip_iperf_udp_client.d" \
"lwip_iperf_udp_server.d" \
"lwip_ping.d" \
"main_freertos.d" \
"network_lwip.d" \
"network_mbedtls.d" \
"network_terminal.d" \
"syscfg\ti_drivers_config.d" \
"syscfg\ti_freertos_config.d" \
"syscfg\ti_freertos_portable_config.d" \
"syscfg\ti_flash_map_config.d" \
"nimble_host.d" \
"nvocmp_cc35xx.d" \
"sntp_task.d" \
"sntp_wrapper.d" \
"socket_examples.d" \
"str.d" \
"wlan_cmd.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_drivers_config.c" \
"syscfg\ti_utils_build_compiler.opt" \
"syscfg\ti_freertos_config.c" \
"syscfg\ti_freertos_portable_config.c" \
"syscfg\ti_flash_map_config.c" 

C_SRCS__QUOTED += \
"../ble_cmd.c" \
"../calibrator.c" \
"../cmd_parser.c" \
"../crc.c" \
"../date_time_service.c" \
"../dhcpserver.c" \
"../lwip_iperf_examples.c" \
"../lwip_iperf_tcp_client.c" \
"../lwip_iperf_tcp_server.c" \
"../lwip_iperf_udp_client.c" \
"../lwip_iperf_udp_server.c" \
"../lwip_ping.c" \
"../main_freertos.c" \
"../network_lwip.c" \
"../network_mbedtls.c" \
"../network_terminal.c" \
"./syscfg/ti_drivers_config.c" \
"./syscfg/ti_freertos_config.c" \
"./syscfg/ti_freertos_portable_config.c" \
"./syscfg/ti_flash_map_config.c" \
"../nimble_host.c" \
"../nvocmp_cc35xx.c" \
"../sntp_task.c" \
"../sntp_wrapper.c" \
"../socket_examples.c" \
"../str.c" \
"../wlan_cmd.c" 

SYSCFG_SRCS__QUOTED += \
"../network_terminal.syscfg" 


