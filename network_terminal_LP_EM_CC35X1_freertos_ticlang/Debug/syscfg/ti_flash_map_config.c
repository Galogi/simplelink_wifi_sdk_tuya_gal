#include <stdint.h>

/* BL2 Slot 1 memory configuration */
uint32_t bl2_physical_slot_1_address = 0x00002000;
uint32_t bl2_logical_slot_1_address = 0xA0002000;
uint32_t bl2_slot_1_region_size = 0x00066000;

/* BL2 Slot 2 memory configuration */
uint32_t bl2_physical_slot_2_address = 0x00068000;
uint32_t bl2_logical_slot_2_address = 0xA0068000;
uint32_t bl2_slot_2_region_size = 0x00000000;

/* WiFi Connectivity Slot 1 memory configuration */
uint32_t wifi_connectivity_physical_slot_1_address = 0x00068000;
uint32_t wifi_connectivity_logical_slot_1_address = 0xA0068000;
uint32_t wifi_connectivity_slot_1_region_size = 0x00072000;

/* WiFi Connectivity Slot 2 memory configuration */
uint32_t wifi_connectivity_physical_slot_2_address = 0x000DA000;
uint32_t wifi_connectivity_logical_slot_2_address = 0xA00DA000;
uint32_t wifi_connectivity_slot_2_region_size = 0x00000000;

/* Vendor Specific memory configuration */
uint32_t vendor_specific_physical_slot_address = 0x000DB000;
uint32_t vendor_specific_logical_slot_address = 0xA00DB000;
uint32_t vendor_specific_region_size = 0x00000000;

/* NVOCMP memory configuration */
uint32_t nvocmp_physical_slot_address = 0x000DB000;
uint32_t nvocmp_logical_slot_address = 0xA00DB000;
uint32_t nvocmp_region_size = 0x00020000;

/* Key Storage memory configuration */
uint32_t key_storage_physical_slot_address = 0x000FB000;
uint32_t key_storage_logical_slot_address = 0xA00FB000;
uint32_t key_storage_region_size = 0x0000A000;

/* Vendor Image Slot 1 memory configuration */
uint32_t vendor_image_physical_slot_1_address = 0x00105000;
uint32_t vendor_image_logical_slot_1_address = 0x14000000;
uint32_t vendor_image_slot_1_region_size = 0x002FB000;

/* Vendor Image Slot 2 memory configuration */
uint32_t vendor_image_physical_slot_2_address = 0x00400000;
uint32_t vendor_image_logical_slot_2_address = 0xA4000000;
uint32_t vendor_image_slot_2_region_size = 0x00000000;
