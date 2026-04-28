/*
 * Copyright (c) 2025 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== SDWFF3.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SD configuration
 */
let devSpecific = {
    config: [
        {
            name        : "numOfDataLines",
            displayName : "Number Of Data Lines",
            default     : '1 Pins',
            options     : [
                { name : '1 Pins' },
                { name : '4 Pins' }
            ]
        },
        {
            name        : "busFreq",
            displayName : "Card Bus Frequency",
            default     : '20MHz',
            options     : [
                 { name : '10MHz' },
                 { name : '20MHz'},
                 { name : '40MHz'}
            ],
            longDescription : `The clock frequency that provided to the SD card
            `
        },
        {
            name        : "CardDetect",
            displayName : "Card Detect Functionality",
            default     : 'Disable',
            options     : [
                 { name : 'Disable' },
                 { name : 'Enable Active High'},
                 { name : 'Enable Active Low'}
            ],
            longDescription : `Define if card-detection (CD) functionality is enabled, Depende on the card slot.

            'Disable': The card slot doesn't supply CD indication
            'Enable Active High': Card slot supply CD indication
                                  * High - card connected
                                  * Low  - card disconnected
            'Enable Active Low' : Card slot supply CD indication
                                  * High - card disconnected
                                  * Low  - card connected
            `
        },
        {
            name        : "WriteProtect",
            displayName : "Write Protect Functionality",
            default     : 'Disable',
            options     : [
                 { name : 'Disable' },
                 { name : 'Enable Active High'},
                 { name : 'Enable Active Low'}
            ],
            longDescription : `Define if write-protection (WP) functionality is enabled, Depende on the card.

            'Disable': The card doesn't supply WP indication
            'Enable Active High': Card supply WP indication
                                  * High - Write disabled
                                  * Low  - Write ensabled
            'Enable Active Low' : Card supply WP indication
                                  * High - Write ensabled
                                  * Low  - Write disabled
            `
        },
        {
            name        : "powerSignal",
            displayName : "Power Indication Functionality",
            default     : 'Disable',
            options     : [
                 { name : 'Disable' },
                 { name : 'Enable, Turn-Off Sleep'},
                 { name : 'Enable, Always On'}
            ],
            longDescription : `This feature controls the power supply to the SD card through a dedicated pad,
            providing different power management options:
            Options:
            Disable -
                    Power is continuously supplied directly to the SD card
                    No ability to disconnect power
                    SD card remains powered at all times
            Enable, Turn-Off Sleep -
                    Power to the SD card is automatically cut when the chip enters sleep mode
                    Requires SD card re-initialization by the host when power is restored
            Enable, Always On -
                    Power to the SD card is maintained even when the chip enters sleep mode
                    SD card remains powered and ready without requiring re-initialization
            Note: When the SD card power is turned off using any method, the host must re-initialize the SD card before use.
            `
        }
    ],

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/sd/SDWFF3.Board.c.xdt",
        boardh : "/ti/drivers/sd/SD.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let clk = "Unassigned";
    let cmd = "Unassigned";
    let data0;
    let data1;
    let data2;
    let data3;
    let cd;
    let wp;
    let pow;

    if (inst.sd) {
        if (inst.sd.clkPin) {
            clk = inst.sd.clkPin.$solution.devicePinName.replace("_", "");
        }
        if (inst.sd.cmdPin) {
            cmd = inst.sd.cmdPin.$solution.devicePinName.replace("_", "");
        }

        pin = "\nCLK: " + clk + "\nCMD: " + cmd;

        if (inst.sd.data0Pin) {
            data0 = inst.sd.data0Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA0: " + data0;
        }
        if (inst.sd.data1Pin) {
            data1 = inst.sd.data1Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA1: " + data1;
        }
        if (inst.sd.data2Pin) {
            data2 = inst.sd.data2Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA2: " + data2;
        }
        if (inst.sd.data3Pin) {
            data3 = inst.sd.data3Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA3: " + data3;
        }
        if (inst.sd.cdPin) {
            cd = inst.sd.cdPin.$solution.devicePinName.replace("_", "");
            pin += "\nCD: " + cd;
        }
        if (inst.sd.wpPin) {
            wp = inst.sd.wpPin.$solution.devicePinName.replace("_", "");
            pin += "\nWP: " + wp;
        }
        if (inst.sd.powPin) {
            pow = inst.sd.powPin.$solution.devicePinName.replace("_", "");
            pin += "\nPOW: " + pow;
        }

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== pinmuxRequirements ========
 *
 *  param inst      - SD instance
 *
 *  returns req[] - array of requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let clk = {
        name           : "clkPin",    /* config script name */
        displayName    : "CLK Pin",   /* GUI name */
        interfaceNames : ["CLK"]      /* pinmux tool name */
    };

    let cmd = {
        name           : "cmdPin",
        displayName    : "CMD Pin",
        interfaceNames : ["CMD"]
    };

    let data0 = {
        name              : "data0Pin",  /* config script name */
        displayName       : "DATA0 Pin", /* GUI name */
        interfaceNames    : ["DATA0"]    /* pinmux tool name */
    };

    let data1 = {
        name              : "data1Pin",  /* config script name */
        displayName       : "DATA1 Pin", /* GUI name */
        interfaceNames    : ["DATA1"]    /* pinmux tool name */
    };

    let data2 = {
        name              : "data2Pin",  /* config script name */
        displayName       : "DATA2 Pin", /* GUI name */
        interfaceNames    : ["DATA2"]    /* pinmux tool name */
    };

    let data3 = {
        name              : "data3Pin",  /* config script name */
        displayName       : "DATA3 Pin", /* GUI name */
        interfaceNames    : ["DATA3"]    /* pinmux tool name */
    };

    let cd = {
        name              : "cdPin",  /* config script name */
        displayName       : "CD Pin", /* GUI name */
        interfaceNames    : ["CD"]    /* pinmux tool name */
    };

    let wp = {
        name              : "wpPin",  /* config script name */
        displayName       : "WP Pin", /* GUI name */
        interfaceNames    : ["WP"]    /* pinmux tool name */
    };

    let pow = {
        name              : "powPin",  /* config script name */
        displayName       : "POW Pin", /* GUI name */
        interfaceNames    : ["POW1"]   /* pinmux tool name */
    };

    let dmaChannel = {
        name: "dmaChannel",
        displayName: "DMA Channel",
        description: "DMA channel used to send and receive data",
        interfaceNames: ["DMA_RX"]
    };

    let resources = [];

    resources.push(clk);
    resources.push(cmd);
    resources.push(data0);

    if (inst.numOfDataLines == '4 Pins') {
        resources.push(data1);
        resources.push(data2);
        resources.push(data3);
    }

    if (inst.CardDetect != 'Disable') {
        resources.push(cd);
    }

    if (inst.WriteProtect != 'Disable') {
        resources.push(wp);
    }

    if (inst.powerSignal != 'Disable') {
        resources.push(pow);
    }

    resources.push(dmaChannel);

    let sd = {
        name          : "sd",
        displayName   : "SD Peripheral",
        interfaceName : "SDMMC",
        resources     : resources
    };

    return [sd];
}


/*
 *  ======== extend ========
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "SD", null,
        [{name: "SDWFF3"}], null);

    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base SD module */
    extend: extend
};
