/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// SPDX-FileCopyrightText: Copyright 2020-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
// SPDX-License-Identifier: Apache-2.0

/*
 *  ======== psa_fwu.c ========
 * This file is a reference template for implementation of the
 * PSA Certified Firmware Update API v1.0.0
 */

//#include <stddef.h>
//#include <stdint.h>
//#include "osi_kernel.h"

#include <stdbool.h>
#include "psa_fwu.h"
#include "otfde_driver.h"
//#include "fw_status.h"
#include "XMEMWFF3.h"
#include "osi_kernel.h"

/* BL2 Slot 1 memory configuration */
extern uint32_t bl2_physical_slot_1_address;
extern uint32_t bl2_logical_slot_1_address;
extern uint32_t bl2_slot_1_region_size;

/* BL2 Slot 2 memory configuration */
extern uint32_t bl2_physical_slot_2_address;
extern uint32_t bl2_logical_slot_2_address;
extern uint32_t bl2_slot_2_region_size;

/* WiFi Connectivity Slot 1 memory configuration */
extern uint32_t wifi_connectivity_physical_slot_1_address;
extern uint32_t wifi_connectivity_logical_slot_1_address;
extern uint32_t wifi_connectivity_slot_1_region_size;

/* WiFi Connectivity Slot 2 memory configuration */
extern uint32_t wifi_connectivity_physical_slot_2_address;
extern uint32_t wifi_connectivity_logical_slot_2_address;
extern uint32_t wifi_connectivity_slot_2_region_size;

/* Vendor Image Slot 1 memory configuration */
extern uint32_t vendor_image_physical_slot_1_address;
extern uint32_t vendor_image_logical_slot_1_address;
extern uint32_t vendor_image_slot_1_region_size;

/* Vendor Image Slot 2 memory configuration */
extern uint32_t vendor_image_physical_slot_2_address;
extern uint32_t vendor_image_logical_slot_2_address;
extern uint32_t vendor_image_slot_2_region_size;

/*****************************************************************************/
/*                           Enums and Defines                               */
/*****************************************************************************/

typedef enum {
    BL2,                 /* ID 0 */
    WSOC_OR_RFTool,      /* ID 1 */
    Vendor_Image,        /* ID 2 */
    //SBL,               /* ID 3 */
    MAX_COMPONENT_TYPES            
} PSA_FWU_Component_TYPES_e;

#define PRIMARY                     (1)
#define NOT_PRIMARY                 (0)
#define ACTIVE                      (1)
#define LOADED                      (0)
#define SECTOR_SIZE                 (4096)

#define SLOT_1_OK                   (0x01)
#define SLOT_2_OK                   (0x02)
#define OTA_VER_CONFLICT            (0x04)
#define ALTERNATE_OTA               (0x08)
#define NO_VALID_MAIN               (0x10)
#define OTA_DONT_CARE               (0x20)
#define BOTH_SLOTS_INVALID          (0x40)
#define AUTH_ERROR                  (0x80)

#define NO_OTA                      (0x0)
#define OTA_VALIDATE                (0x1)
#define OTA_COMMIT                  (0x2)
#define OTA_COMMIT_AND_PROTECT      (0x3)

#define SUCCESS                     (0x0)
#define ROLLBACK_NEXT_BOOT          (0x1)
#define GENERAL_ERROR               (0x2)
#define SECURITY_FAILURE            (0x3)

#define READ_STATUS_SUCCESS         (0x00)
#define READ_STATUS_SECONDARY_SLOT  (0x01)
#define READ_STATUS_NO_OTA          (0x02)
#define READ_STATUS_ALREADY_PRIMARY (0x03)
#define READ_STATUS_NOT_SUPPORTED   (0x04)
#define READ_STATUS_ERROR           (0x05)

#define GPE_ACTIVE                  (0x0)
#define GPE_STAGED                  (0x1)
#define GPE_DIRTY                   (0x2)
#define GPE_ERASED                  (0x3)

#define MAGIC_MISSING               (0x0)
#define MAGIC_EXISTS                (0x1)

#define COMPONENT_READ_STATUS_OK     (0x0)
#define COMPONENT_READ_STATUS_EMPTY  (0x1)
#define COMPONENT_READ_STATUS_FAIL   (0x2)

#define COMPONENT_FLOW_STATE_RESET_VALIDATE  (8)
#define COMPONENT_FLOW_STATE_RESET_COMMIT    (9)

#define IS_VALID_COMPONENT_ID(componentID)                        ( componentID < MAX_COMPONENT_ID )
#define IS_COMPONENT_ID_PRIMARY(componentID)                      ( gImageStore[componentID].ComponentInfo.impl.Primary == PRIMARY )
#define UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(state, error)        update_ComponentInfo(component, state, error); \
                                                                    return error;
#define PRCM_AON_NON_SECURED__PRCM_CORE_STOP_ENABLE__ADDR    (0x41091060)
#define PRCM_SCRATCHPAD__PRCM_SCPAD2__ADDR                   (0x41099000)
#define PRCM_AON__RST_CTRL__ADDR                             (0x410920b4)
#define REGISTER(address) (*(volatile uint32 *)(address))


extern uint32_t _Boot_report_address;
#define BOOT_REPORT_ADDR ((uint32_t) (&_Boot_report_address))

/*****************************************************************************/
/*                           Structures                                      */
/*****************************************************************************/
typedef union
{
    struct
    {
        uint16_t BL2                      : 3; /* BITS:[0:2] */
        uint16_t WSOC_OR_RFTOOL           : 3; /* BITS:[3:5] */
        uint16_t SBL                      : 3; /* BITS:[6:8] */
        uint16_t VENDOR_IMAGE             : 3; /* BITS:[9:11] */
        uint16_t START_MODE               : 3; /* BITS:[12:14] */
        uint16_t M33_REQUEST              : 1; /* BITS:[15:15] */
    } FIELD;
    uint16_t ALL;
} PSA_FWU_SCRATCHPAD_FOR_TYPES;

typedef union
{
    struct
    {
        uint8_t REQ_TYPE    : 2; /* BITS:[0-1] */
        uint8_t REQ_STATUS  : 6; /* BITS:[2-7] */
    } FIELD;
    uint8_t ALL;
} PSA_FWU_Request_Report;

typedef struct
{
    PSA_FWU_Request_Report  Logical_Slot_Address;
    uint8_t                 Image_Select_Report;
} PSA_FWU_Report_Structure_t;

typedef struct
{
    uint32_t  Physical_Slot_Address;
    uint32_t  Logical_Slot_Address;
    uint32_t  Region_Size;
} PSA_FWU_Slot_Address_t;

/* This struct holds the starting pointer for the slot of specific ComponentID
   and if this slot is primary or not */
typedef struct
{
    psa_fwu_component_t      ComponentID;
    psa_fwu_component_info_t ComponentInfo;
    XMEM_Handle              XMEMhandle;
} PSA_FWU_Slot_Info_t;

/*****************************************************************************/
/*                           Global parameters                               */
/*****************************************************************************/
PSA_FWU_Magic_t gGPEMagicNumber[MAX_COMPONENT_TYPES] = {
    0x690c47c2 ,
    0x690c47c2 ,
    0x690c47c2
};
//updated magic numbers

PSA_FWU_Slot_Address_t gSlotAddress[MAX_COMPONENT_ID];

PSA_FWU_Slot_Address_t gSlotVendorAlternate[2];

PSA_FWU_Report_Structure_t gOTAReport[MAX_COMPONENT_TYPES];

PSA_FWU_Slot_Info_t gImageStore[MAX_COMPONENT_ID] = {0};

static bool isInitialized = false;


/*****************************************************************************/
/*                      Utility Functions Prototypes                         */
/*****************************************************************************/
uint32_t     Retrieve_active_and_loaded_components(psa_fwu_component_t component, psa_fwu_component_t *active_component, psa_fwu_component_t *loaded_component);
void         Create_component_XMEMHandle(psa_fwu_component_t component, uint32_t readStatus);
uint8_t      Magic_validation_and_coping_version(psa_fwu_component_t component);
psa_status_t is_manifest_integrity_valid(uint32_t *manifest);
psa_status_t is_GPE_magic_number_valid(psa_fwu_component_t component, PSA_FWU_Magic_t *GPE_magic_number);
bool         isVersionNewer(psa_fwu_image_version_t* v1, psa_fwu_image_version_t* v2);
uint32_t     read_component_state_from_flash(psa_fwu_component_t component, int8_t *state, int8_t *reset_state);
psa_status_t write_component_multiple_state_in_flash(psa_fwu_component_t component, uint8_t states);
psa_status_t write_component_state_in_flash(psa_fwu_component_t component, uint8_t state);
void         update_ComponentGPEState(psa_fwu_component_t component, uint8_t state);
void         update_ComponentInfo(psa_fwu_component_t component, uint8_t state, psa_status_t error);
psa_status_t validation_check(psa_fwu_component_t component, uint8_t state);
psa_status_t flow_check(psa_fwu_component_t component, uint8_t state);
psa_status_t erase_GPE(psa_fwu_component_t component);
uint32_t     Read_report_and_update_status(psa_fwu_component_t component);
void         update_component_loaded_state(psa_fwu_component_t component, uint32_t readReportStatus);
void         initialize_slot_addresses(void);

/*****************************************************************************/
/*                      Utility Functions implementation                     */
/*****************************************************************************/
psa_status_t is_manifest_integrity_valid(uint32_t *manifest)
{
    uint8_t Index;
    
    for (Index = 0; Index < MANIFEST_INTEGRITY_SIZE_IN_WORDS; Index++)
    {
        if (manifest[Index] != 0xFFFFFFFF)
        {
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_DATA_INVALID;
}

psa_status_t is_GPE_magic_number_valid(psa_fwu_component_t component, PSA_FWU_Magic_t *GPE_magic_number)
{
    uint8_t Index;
    
    if (GPE_magic_number->Magic_Number != gGPEMagicNumber[component >> 1].Magic_Number)
    {
        return PSA_ERROR_DATA_INVALID;
    }
    return PSA_SUCCESS;
}

bool isVersionNewer(psa_fwu_image_version_t* v1, psa_fwu_image_version_t* v2)
{
    if (v1->major != v2->major) return v1->major > v2->major;
    if (v1->minor != v2->minor) return v1->minor > v2->minor;
    if (v1->patch != v2->patch) return v1->patch > v2->patch;
    return v1->build > v2->build;
}

uint32_t read_component_state_from_flash(psa_fwu_component_t component, int8_t *state, int8_t *reset_state)
{
    int_fast16_t retXMEM;
    uint32_t flow_state;

    retXMEM = XMEMWFF3_read(gImageStore[component].XMEMhandle, SECTOR_SIZE - ( TI_FWU_MAGIC_NUMBER_SIZE + TI_FWU_MANIFEST_INTEGRITY_SIZE + sizeof(uint32_t) ), &(flow_state), sizeof(uint32_t), XMEM_READ_STIG);
    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        *state = PSA_FWU_FAILED;
        return COMPONENT_READ_STATUS_FAIL;
    }

    if ( flow_state == 0xFFFFFFFF )
    {
        *state = PSA_FWU_FAILED;
        return COMPONENT_READ_STATUS_EMPTY;
    }

    if ((~flow_state) & (1 << COMPONENT_FLOW_STATE_RESET_VALIDATE))
    {
        *reset_state = COMPONENT_FLOW_STATE_RESET_VALIDATE;
    }
    else if ((~flow_state) & (1 << COMPONENT_FLOW_STATE_RESET_COMMIT))
    {
        *reset_state = COMPONENT_FLOW_STATE_RESET_COMMIT;
    }



    if ((~flow_state) & (1 << PSA_FWU_REJECTED))
    {
        *state = PSA_FWU_REJECTED;
    }
    else if ((~flow_state) & (1 << PSA_FWU_FAILED))
    {
        *state = PSA_FWU_FAILED;
    }
    else 
    {
        for (*state = 7; *state > 0; (*state)--)
        {
            if ( !(flow_state & (1 << (*state))) )
            {
                break;
            }
        }

    }

    return COMPONENT_READ_STATUS_OK;
}

psa_status_t write_component_multiple_state_in_flash(psa_fwu_component_t component, uint8_t states)
{
    int_fast16_t retXMEM;
    uint32_t flow_state = (0xFFFFFFFF & ~(states));

    retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE - ( TI_FWU_MAGIC_NUMBER_SIZE + TI_FWU_MANIFEST_INTEGRITY_SIZE + sizeof(uint32_t) ), &(flow_state), sizeof(uint32_t), XMEM_WRITE_STIG);
    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        return PSA_ERROR_DATA_INVALID;
    }

    return PSA_SUCCESS;
}

psa_status_t write_component_state_in_flash(psa_fwu_component_t component, uint8_t state)
{
    int_fast16_t retXMEM;
    uint32_t flow_state = (0xFFFFFFFF & ~(1 << state));

    retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE - ( TI_FWU_MAGIC_NUMBER_SIZE + TI_FWU_MANIFEST_INTEGRITY_SIZE + sizeof(uint32_t) ), &(flow_state), sizeof(uint32_t), XMEM_WRITE_STIG);
    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        return PSA_ERROR_DATA_INVALID;
    }

    return PSA_SUCCESS;
}

psa_status_t validation_check(psa_fwu_component_t component, uint8_t state)
{
    if (!(IS_VALID_COMPONENT_ID(component)))
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(state ,PSA_ERROR_INVALID_HANDLE)
    }

    if (IS_COMPONENT_ID_PRIMARY(component))
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(state ,PSA_ERROR_ALREADY_EXISTS)
    }
    return PSA_SUCCESS;
}

psa_status_t flow_check(psa_fwu_component_t component, uint8_t state)
{
    if (gImageStore[component].ComponentInfo.state != state)
    {
        return PSA_ERROR_BAD_STATE;
    }

    return PSA_SUCCESS;
}

psa_status_t erase_GPE(psa_fwu_component_t component)
{
    int_fast16_t retXMEM;
    uint32_t SectorIndex = 1;

    while (SectorIndex < ((gSlotAddress[component].Region_Size) >> 12))
    {
        retXMEM = XMEMWFF3_erase(gImageStore[component].XMEMhandle, SECTOR_SIZE * SectorIndex, SECTOR_SIZE);
        if (retXMEM != XMEM_STATUS_SUCCESS)
        {
            UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_FAILED ,PSA_ERROR_DATA_INVALID)
        }
        SectorIndex++;
    }

    retXMEM = XMEMWFF3_erase(gImageStore[component].XMEMhandle, 0, SECTOR_SIZE);
    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_FAILED ,PSA_ERROR_DATA_INVALID)
    }
    return PSA_SUCCESS;
}


uint32_t Retrieve_active_and_loaded_components(psa_fwu_component_t component, psa_fwu_component_t *active_component, psa_fwu_component_t *loaded_component)
{
    if (gOTAReport[component>>1].Image_Select_Report & SLOT_1_OK)
    {
        *active_component = component;
        *loaded_component = component ^ 0x1;
    }
    else if (gOTAReport[component>>1].Image_Select_Report & SLOT_2_OK)
    {
        *active_component = component ^ 0x1;
        *loaded_component = component;
        return READ_STATUS_SECONDARY_SLOT;
    }
    else
    {
        return READ_STATUS_ERROR;
    }
    return READ_STATUS_SUCCESS;
}

void Create_component_XMEMHandle(psa_fwu_component_t component, uint32_t readStatus)
{
    psa_fwu_component_t VendorImage_component;
    XMEM_Params params;
    if ((readStatus == READ_STATUS_SECONDARY_SLOT) && (component >= Vendor_Image_Slot_1))
    {
        VendorImage_component = component - Vendor_Image_Slot_1;
        params.regionBase = gSlotVendorAlternate[VendorImage_component].Physical_Slot_Address;
        params.regionStartAddr = gSlotVendorAlternate[VendorImage_component].Logical_Slot_Address;
        params.regionSize = gSlotVendorAlternate[VendorImage_component].Region_Size;
        params.deviceNum = 0;
        gImageStore[component].XMEMhandle = XMEMWFF3_open(&params);
    }
    else 
    {
        params.regionBase = gSlotAddress[component].Physical_Slot_Address;
        params.regionStartAddr = gSlotAddress[component].Logical_Slot_Address;
        params.regionSize = gSlotAddress[component].Region_Size;
        params.deviceNum = 0;
        gImageStore[component].XMEMhandle = XMEMWFF3_open(&params);
    }
}
uint8_t Magic_validation_and_coping_version(psa_fwu_component_t component)

{
    int_fast16_t retXMEM;
    PSA_FWU_GPESlot_t GPE_Manifest; 

    retXMEM = XMEMWFF3_read(gImageStore[component].XMEMhandle, SECTOR_SIZE - TI_FWU_MAGIC_NUMBER_SIZE, &(GPE_Manifest.Manifest.GPE_Header.ih_magic.Magic_Number), TI_FWU_MAGIC_NUMBER_SIZE, XMEM_READ_STIG);
    if (retXMEM == XMEM_STATUS_SUCCESS)
    {
        if (is_GPE_magic_number_valid(component, &(GPE_Manifest.Manifest.GPE_Header.ih_magic)) == PSA_SUCCESS)
        {
            retXMEM = XMEMWFF3_read(gImageStore[component].XMEMhandle, SECTOR_SIZE, (void *)((uint8_t *)&(GPE_Manifest.Manifest) + TI_FWU_MAGIC_NUMBER_SIZE), sizeof(PSA_FWU_GPEManifest_t) - TI_FWU_MAGIC_NUMBER_SIZE, 0);
            if (retXMEM == XMEM_STATUS_SUCCESS)
            {
                if (is_GPE_magic_number_valid(component, &(GPE_Manifest.Manifest.GPE_Header.ih_magic)) != PSA_SUCCESS)
                {
                    return MAGIC_MISSING;
                }
                else 
                {
                    os_memcpy(&(gImageStore[component].ComponentInfo.version), &(GPE_Manifest.Manifest.GPE_Version), sizeof(PSA_FWU_GPEVersion_t));    
                    return MAGIC_EXISTS;
                }
            }
        }
    }
    return MAGIC_MISSING;
}

void update_ComponentGPEState(psa_fwu_component_t component, uint8_t state)
{
    switch (state)
    {
        case PSA_FWU_UPDATED:
        {
            gImageStore[component].ComponentInfo.impl.GPE_state = GPE_ACTIVE;
        }
        break;
        case PSA_FWU_WRITING:
        case PSA_FWU_STAGED:
        case PSA_FWU_CANDIDATE:
        case PSA_FWU_TRIAL:
        {
            gImageStore[component].ComponentInfo.impl.GPE_state = GPE_STAGED;
        }
        break;
        case PSA_FWU_FAILED:
        case PSA_FWU_REJECTED:
        {
            gImageStore[component].ComponentInfo.impl.GPE_state = GPE_DIRTY;
        }
        break;
        case PSA_FWU_READY:
        {
            gImageStore[component].ComponentInfo.impl.GPE_state = GPE_ERASED;
        }
        break;
    }
    return;
}

void update_ComponentInfo(psa_fwu_component_t component, uint8_t state, psa_status_t error)
{
    gImageStore[component].ComponentInfo.error = error;
    
    if (error == PSA_SUCCESS)
    {
        gImageStore[component].ComponentInfo.state = state;
        write_component_state_in_flash(component, state);

        update_ComponentGPEState(component, state);
    }
    return;
}

uint32_t Read_report_and_update_status(psa_fwu_component_t component)
{
    int8_t flash_state;
    int8_t reset_state;
    uint32_t readStateRetVal;

    psa_fwu_component_info_t *info = &(gImageStore[component].ComponentInfo);

    
    info->impl.request_type = gOTAReport[component>>1].Logical_Slot_Address.FIELD.REQ_TYPE;
    info->impl.extended_report_status = gOTAReport[component>>1].Image_Select_Report;

    uint8_t magic_exists = Magic_validation_and_coping_version(component);
    readStateRetVal = read_component_state_from_flash(component, &flash_state, &reset_state);

    info->impl.running_status = ACTIVE;

    if (readStateRetVal == COMPONENT_READ_STATUS_EMPTY)
    {
        flash_state = (1 << PSA_FWU_TRIAL) | (1 << PSA_FWU_STAGED) | (1 << PSA_FWU_CANDIDATE) | (1 << PSA_FWU_WRITING) | (1 << PSA_FWU_READY);
        write_component_multiple_state_in_flash(component, flash_state);
    }

    if (gOTAReport[component>>1].Logical_Slot_Address.FIELD.REQ_TYPE == NO_OTA)
    {
        info->error = PSA_SUCCESS;
        info->impl.Primary = PRIMARY;
        info->state = PSA_FWU_UPDATED;
        update_ComponentInfo(component, info->state, info->error);
        return READ_STATUS_NO_OTA;
    }
    else
    {
        if (gOTAReport[component>>1].Logical_Slot_Address.FIELD.REQ_STATUS == SUCCESS)
        {
            if (info->impl.request_type == OTA_VALIDATE)
            {
                info->impl.Primary = NOT_PRIMARY;
                info->state = PSA_FWU_TRIAL;
            }
            else if (info->impl.request_type == OTA_COMMIT)
            {
                info->impl.Primary = PRIMARY;
                info->state = PSA_FWU_UPDATED;
            }
            else if (info->impl.request_type == OTA_COMMIT_AND_PROTECT)
            {
                info->impl.Primary = PRIMARY;
                info->state = PSA_FWU_UPDATED;
            }
            info->error = PSA_SUCCESS;
        }
        else if (gOTAReport[component>>1].Logical_Slot_Address.FIELD.REQ_STATUS != SUCCESS) 
        {
            //Think more
            if (gOTAReport[component>>1].Logical_Slot_Address.FIELD.REQ_STATUS == SECURITY_FAILURE) 
            {
                info->error = PSA_ERROR_INVALID_SIGNATURE;
            }
            else 
            {
                info->error = PSA_ERROR_PROGRAMMER_ERROR;
            }
            info->impl.Primary = NOT_PRIMARY;
        }
        update_ComponentInfo(component, info->state, info->error);
    }

    return READ_STATUS_SUCCESS;
}

void update_component_loaded_state(psa_fwu_component_t component, uint32_t readReportStatus)
{
    psa_status_t retVal = PSA_SUCCESS;
    uint8_t state;
    uint8_t magic_exists;
    int8_t flash_state;
    int8_t reset_state;
    uint32_t readStateRetVal;
    
    gImageStore[component].ComponentInfo.impl.running_status = LOADED;

    magic_exists = Magic_validation_and_coping_version(component);
    readStateRetVal = read_component_state_from_flash(component, &flash_state, &reset_state);

    gImageStore[component].ComponentInfo.state = flash_state;

    if ( (readStateRetVal == COMPONENT_READ_STATUS_EMPTY) && (magic_exists == MAGIC_EXISTS) )
    {
        gImageStore[component].ComponentInfo.impl.GPE_state = GPE_STAGED;
        state = (1 << PSA_FWU_CANDIDATE) | (1 << PSA_FWU_WRITING) | (1 << PSA_FWU_READY);
        write_component_multiple_state_in_flash(component, state);
        update_ComponentInfo(component, PSA_FWU_CANDIDATE, PSA_SUCCESS);
        return;
    }
    else if (readStateRetVal == COMPONENT_READ_STATUS_OK)
    {
        if (magic_exists == MAGIC_EXISTS) 
        {
            update_ComponentGPEState(component, flash_state);
            if ( (readReportStatus == READ_STATUS_NO_OTA) &&
                 ( ( (reset_state == COMPONENT_FLOW_STATE_RESET_VALIDATE) && (flow_check(component, PSA_FWU_TRIAL) == PSA_ERROR_BAD_STATE) ) ||
                   ( (reset_state == COMPONENT_FLOW_STATE_RESET_COMMIT) && (flow_check(component, PSA_FWU_UPDATED) == PSA_ERROR_BAD_STATE) ) ) )
            {
                gImageStore[component].ComponentInfo.error = PSA_ERROR_CORRUPTION_DETECTED;
                return;
            }
            else if (flow_check(component, PSA_FWU_UPDATED) == PSA_SUCCESS)
            {
                gImageStore[component].ComponentInfo.impl.GPE_state = GPE_STAGED;
                if (readReportStatus != READ_STATUS_NO_OTA)
                {
                    gImageStore[component].ComponentInfo.impl.Primary = PRIMARY;
                }
            }
            gImageStore[component].ComponentInfo.error = PSA_SUCCESS;
            return;
        } 
    }
    
    if (flow_check(component, PSA_FWU_READY) == PSA_SUCCESS)
    {
        gImageStore[component].ComponentInfo.impl.GPE_state = GPE_ERASED;
        gImageStore[component].ComponentInfo.error = PSA_SUCCESS;
        gImageStore[component].ComponentInfo.state = PSA_FWU_READY;
        return;
    }
 
    update_ComponentInfo(component, PSA_FWU_REJECTED, PSA_SUCCESS);
    return;
}

void initialize_slot_addresses(void) 
{
    // BL2 Slot 1
    gSlotAddress[0].Physical_Slot_Address = bl2_physical_slot_1_address;
    gSlotAddress[0].Logical_Slot_Address = bl2_logical_slot_1_address;
    gSlotAddress[0].Region_Size = bl2_slot_1_region_size;

    // BL2 Slot 2
    gSlotAddress[1].Physical_Slot_Address = bl2_physical_slot_2_address;
    gSlotAddress[1].Logical_Slot_Address = bl2_logical_slot_2_address;
    gSlotAddress[1].Region_Size = bl2_slot_2_region_size;

    // WiFi Connectivity Slot 1
    gSlotAddress[2].Physical_Slot_Address = wifi_connectivity_physical_slot_1_address;
    gSlotAddress[2].Logical_Slot_Address = wifi_connectivity_logical_slot_1_address;
    gSlotAddress[2].Region_Size = wifi_connectivity_slot_1_region_size;

    // WiFi Connectivity Slot 2
    gSlotAddress[3].Physical_Slot_Address = wifi_connectivity_physical_slot_2_address;
    gSlotAddress[3].Logical_Slot_Address = wifi_connectivity_logical_slot_2_address;
    gSlotAddress[3].Region_Size = wifi_connectivity_slot_2_region_size;

    // Vendor Image Slot 1
    gSlotAddress[4].Physical_Slot_Address = vendor_image_physical_slot_1_address;
    gSlotAddress[4].Logical_Slot_Address = vendor_image_logical_slot_1_address;
    gSlotAddress[4].Region_Size = vendor_image_slot_1_region_size;

    // Vendor Image Slot 2
    gSlotAddress[5].Physical_Slot_Address = vendor_image_physical_slot_2_address;
    gSlotAddress[5].Logical_Slot_Address = vendor_image_logical_slot_2_address;
    gSlotAddress[5].Region_Size = vendor_image_slot_2_region_size;

    // Vendor Alternate Slot configurations
    gSlotVendorAlternate[0].Physical_Slot_Address = vendor_image_physical_slot_1_address;
    gSlotVendorAlternate[0].Logical_Slot_Address = vendor_image_logical_slot_2_address;
    gSlotVendorAlternate[0].Region_Size = vendor_image_slot_2_region_size;

    gSlotVendorAlternate[1].Physical_Slot_Address = vendor_image_physical_slot_2_address;
    gSlotVendorAlternate[1].Logical_Slot_Address = vendor_image_logical_slot_1_address;
    gSlotVendorAlternate[1].Region_Size = vendor_image_slot_1_region_size;
}

/*****************************************************************************/
/*                           API Functions implementation                    */
/*****************************************************************************/

psa_status_t psa_fwu_query(psa_fwu_component_t component,
                           psa_fwu_component_info_t *info)
{
    if (!(IS_VALID_COMPONENT_ID(component)))
    {
        return PSA_ERROR_INVALID_HANDLE;
    }
 
    os_memcpy(info, &(gImageStore[component].ComponentInfo), sizeof(psa_fwu_component_info_t));

    return PSA_SUCCESS;
}
                           
psa_status_t psa_fwu_start(psa_fwu_component_t component,
                           const void *manifest,
                           size_t manifest_size)
{

    int_fast16_t retXMEM;
    PSA_FWU_GPESlot_t *Manifest = (PSA_FWU_GPESlot_t *)manifest;
    uint8_t Index;
    psa_status_t retVal;

    retVal = validation_check(component, gImageStore[component].ComponentInfo.state);
    if (retVal != PSA_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_READY ,retVal)
    }

    /* Check that this function is called after the component is in PSA_FWU_READY state */
    retVal = flow_check(component, PSA_FWU_READY);
    if (retVal != PSA_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_READY ,retVal)
    }
    
    retVal =  is_manifest_integrity_valid((uint32_t*)Manifest);
    if (retVal != PSA_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_READY ,retVal)
    }

    retVal = is_GPE_magic_number_valid(component, &(Manifest->Manifest.GPE_Magic));
    if (retVal != PSA_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_READY ,retVal)
    }

    /* In order to complete loading new version to slot, it must be a higher version */
    psa_fwu_image_version_t *secondaryVersion  = (psa_fwu_image_version_t*)&(Manifest->Manifest.GPE_Version);
    psa_fwu_image_version_t *primaryVersion = &(gImageStore[component ^ 0x01].ComponentInfo.version);

    /* Compare versions in order of significance (major, minor, patch, build) */
    if (!isVersionNewer(secondaryVersion , primaryVersion))
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_READY ,PSA_ERROR_DATA_INVALID)
    }

    /* Write manifest integrity*/
    /* Update state in flash before writing starts in order to know that the flash
       won't be clean */
    write_component_state_in_flash(component, PSA_FWU_WRITING);
    for (Index = 0; Index < MANIFEST_INTEGRITY_SIZE_IN_WORDS; Index++)
    {
        retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE - TI_FWU_MAGIC_NUMBER_SIZE - TI_FWU_MANIFEST_INTEGRITY_SIZE + Index*4, (void *)&(Manifest->Manifest_Integrity.Manifest_Integrity[Index]), sizeof(uint32_t), XMEM_WRITE_STIG);
        if (retXMEM != XMEM_STATUS_SUCCESS)
        {
            UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,PSA_ERROR_DATA_INVALID)
        }
    }

    if (component == Vendor_Image_Slot_1)
    {
        otfdeDriver_Config(0, (const uint8_t*)Manifest->Manifest_Integrity.Manifest_Integrity, TI_FWU_MANIFEST_INTEGRITY_SIZE);
    }
    else if (component == BL2_Slot_1)
    {
        otfdeDriver_Config(1, (const uint8_t*)Manifest->Manifest_Integrity.Manifest_Integrity, TI_FWU_MANIFEST_INTEGRITY_SIZE);
    }
    else if (component == Vendor_Image_Slot_2)
    {
        otfdeDriver_Config(2, (const uint8_t*)Manifest->Manifest_Integrity.Manifest_Integrity, TI_FWU_MANIFEST_INTEGRITY_SIZE);
    }
    
    /* Write manifest without ih_magic */
    retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE, (void *)(uint8 *)Manifest + TI_FWU_MANIFEST_INTEGRITY_SIZE + TI_FWU_MAGIC_NUMBER_SIZE, manifest_size  - TI_FWU_MAGIC_NUMBER_SIZE - TI_FWU_MANIFEST_INTEGRITY_SIZE, 0);

    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,PSA_ERROR_DATA_INVALID)
    }

    /* All required steps passed, update version */
    os_memcpy(&(gImageStore[component].ComponentInfo.version), &(Manifest->Manifest.GPE_Version), sizeof(PSA_FWU_GPEVersion_t));

    /* All required steps passed, update state */
    UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,PSA_SUCCESS)

}


psa_status_t psa_fwu_write(psa_fwu_component_t component,
                           size_t image_offset,
                           const void *block,
                           size_t block_size)
{
    int_fast16_t retXMEM;
    psa_status_t retVal;

    retVal = validation_check(component,PSA_FWU_WRITING);
    if (retVal != PSA_SUCCESS)
    {
        return retVal;
    }

    /* Check that this function is called after the component is in PSA_FWU_WRITING state */
    retVal = flow_check(component, PSA_FWU_WRITING);
    if (retVal != PSA_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,retVal)
    }

    retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE + image_offset, (void *)block, block_size, 0);
    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,PSA_ERROR_DATA_INVALID)
    }

    /* All required steps passed, update state */
    UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,PSA_SUCCESS)
}


psa_status_t psa_fwu_finish(psa_fwu_component_t component)
{
    int_fast16_t retXMEM;
    psa_status_t retVal;

    retVal = validation_check(component, PSA_FWU_WRITING);
    if (retVal != PSA_SUCCESS)
    {
        return retVal;
    }

    /* Check that this function is called after the component is in PSA_FWU_WRITING state */
    retVal = flow_check(component, PSA_FWU_WRITING);
    if (retVal != PSA_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,retVal)
    }

    retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE - TI_FWU_MAGIC_NUMBER_SIZE, &(gGPEMagicNumber[component >> 1].Magic_Number), TI_FWU_MAGIC_NUMBER_SIZE, XMEM_WRITE_STIG);
    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_WRITING ,PSA_ERROR_DATA_INVALID)
    }

    /* All required steps passed, update state */
    UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_CANDIDATE ,PSA_SUCCESS)
}


psa_status_t psa_fwu_cancel(psa_fwu_component_t component)
{
    int_fast16_t retXMEM;
    PSA_FWU_Magic_t Emptymagic = {0};
    psa_status_t retVal;

    retVal = validation_check(component, gImageStore[component].ComponentInfo.state);
    if (retVal != PSA_SUCCESS)
    {
        return retVal;
    }

    if (gImageStore[component].ComponentInfo.impl.running_status == ACTIVE)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(gImageStore[component].ComponentInfo.state ,PSA_ERROR_BAD_STATE)
    }

    retXMEM = XMEMWFF3_write(gImageStore[component].XMEMhandle, SECTOR_SIZE - TI_FWU_MAGIC_NUMBER_SIZE, &(Emptymagic.Magic_Number), TI_FWU_MAGIC_NUMBER_SIZE, XMEM_WRITE_STIG);

    if (retXMEM != XMEM_STATUS_SUCCESS)
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_FAILED ,PSA_ERROR_DATA_INVALID)
    }

    /* All required steps passed, update state */
    UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_FAILED ,PSA_SUCCESS)
}


psa_status_t psa_fwu_clean(psa_fwu_component_t component)
{
    psa_status_t retVal;

    retVal = validation_check(component, gImageStore[component].ComponentInfo.state);
    if (retVal != PSA_SUCCESS)
    {
        return retVal;
    }

    /* Previous state must be REJECTED or FAILED before cleaning */
    retVal = flow_check(component, PSA_FWU_REJECTED);
    if (retVal != PSA_SUCCESS)
    {
        retVal = flow_check(component, PSA_FWU_FAILED);
        if (retVal != PSA_SUCCESS)
        {
            UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(gImageStore[component].ComponentInfo.state ,retVal)
        }
    }

    retVal = erase_GPE(component);
    if (retVal != PSA_SUCCESS)
    {
        return retVal;
    }

    os_memset(&gImageStore[component].ComponentInfo.version, 0, sizeof(psa_fwu_image_version_t));

    /* All required steps passed, update state */
    UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_READY ,PSA_SUCCESS)

}

psa_status_t psa_fwu_install(void)
{
    psa_fwu_component_t component;
    bool isAtLeastOneCandidate = false;

    for (component = 0; component < MAX_COMPONENT_ID; component++ )
    {
        /* If component is not ready, skip */
        if (PSA_SUCCESS == flow_check(component, PSA_FWU_CANDIDATE))
        {
            update_ComponentInfo(component, PSA_FWU_STAGED, PSA_SUCCESS);
            isAtLeastOneCandidate = true;
        }
    }

    if (!isAtLeastOneCandidate)
    {
        return PSA_ERROR_BAD_STATE;
    }

    return PSA_SUCCESS;
}


psa_status_t psa_fwu_request_reboot(void)
{
    psa_fwu_component_t           component;
    uint8_t                       resetNeeded = 0;
    uint8_t                       slotIndex;
    PSA_FWU_SCRATCHPAD_FOR_TYPES  scratchpad = {0};
    uint16_t                      Request_type[MAX_COMPONENT_TYPES] = {0};

    for (component = 0; component < MAX_COMPONENT_ID;)
    {
        Request_type[component>>1] = 0x0;
        //fix
        for (slotIndex = 0; slotIndex < 2; component++, slotIndex++)
        {
            if (gImageStore[component].ComponentInfo.state == PSA_FWU_STAGED)
            {
                write_component_state_in_flash(component, COMPONENT_FLOW_STATE_RESET_VALIDATE);
                Request_type[component>>1] |= OTA_VALIDATE;
                resetNeeded = 1;
            }
            else if (gImageStore[component].ComponentInfo.state == PSA_FWU_TRIAL)
            {
                write_component_state_in_flash(component, COMPONENT_FLOW_STATE_RESET_COMMIT);
                Request_type[component>>1] |= OTA_COMMIT;
                resetNeeded = 1;
            }
        }
    }

    if (0 == resetNeeded)
    {
        return PSA_ERROR_BAD_STATE;
    }

    scratchpad.FIELD.BL2 = Request_type[BL2];
    scratchpad.FIELD.WSOC_OR_RFTOOL = Request_type[WSOC_OR_RFTool];
    scratchpad.FIELD.VENDOR_IMAGE = Request_type[Vendor_Image];

    REGISTER(PRCM_SCRATCHPAD__PRCM_SCPAD2__ADDR) = scratchpad.ALL;

    REGISTER(PRCM_AON_NON_SECURED__PRCM_CORE_STOP_ENABLE__ADDR) = 0;

    osi_uSleep(1000);

    REGISTER(PRCM_AON__RST_CTRL__ADDR) = 0x1;

    return PSA_SUCCESS;
}

void psa_fwu_init(void)
{
    PSA_FWU_Report_Structure_t *pSharedReport = (PSA_FWU_Report_Structure_t *)BOOT_REPORT_ADDR;
    psa_fwu_component_t component;
    psa_fwu_component_t active_component;
    psa_fwu_component_t loaded_component;
    uint32_t readStatus;
    uint32_t readReportStatus;
    uint8_t magic_exists = MAGIC_MISSING;

    if (!isInitialized)
    {
        isInitialized = (bool)true;

        initialize_slot_addresses();

        os_memcpy(gOTAReport, pSharedReport, sizeof(gOTAReport));

        os_memset(pSharedReport, 0, sizeof(gOTAReport));

        for (component = 0; component < MAX_COMPONENT_ID; component+=2 )
        {
            readStatus = Retrieve_active_and_loaded_components(component, &active_component, &loaded_component);

            Create_component_XMEMHandle(active_component, readStatus);
            Create_component_XMEMHandle(loaded_component, readStatus);

            /* Update active component fields */
            readReportStatus = Read_report_and_update_status(active_component);

            /* Update loaded component fields */
            update_component_loaded_state(loaded_component, readReportStatus);
        }
    }
}


#if 0
psa_status_t psa_fwu_reject(psa_status_t error)
{
    psa_fwu_component_t component;

    psa_status_t retVal = validation_check(component);
    if (retVal != PSA_SUCCESS)
    {
        return retVal;
    }
    
    /* Previous state must be STAGED or TRIAL before cleaning */
    if ( (gImageStore[component].ComponentInfo.state != PSA_FWU_STAGED) && 
         (gImageStore[component].ComponentInfo.state != PSA_FWU_TRIAL) )
    {
        UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_REJECTED ,PSA_ERROR_BAD_STATE)
    }


    /* All required steps passed, update state */
    UPDATE_COMPONENT_INFO_AND_RETURN_ERROR(PSA_FWU_REJECTED ,PSA_SUCCESS)

}


psa_status_t psa_fwu_accept(void)
{

}
#endif
