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
 *  ======== SDIOWFF3.syscfg.js ========
 */

"use strict";

/* Get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SDIO configuration
 */
let devSpecific = {
    config: [
        {
            name        : "irqSignal",
            displayName : "IRQ Band",
            default     : 'In-band',
            options     : [
                 { name : 'In-band' },
                 { name : 'Out-of-band'}
            ],
            longDescription : `IRQ from SDIO peripheral to host could be done in 2 ways:\n
        * In-band IRQ (ibi) - IRQ will be driven on data line 1\n
        * Out-of-band IRQ (obi) - saperated pin for IRQ\n
    IRQ implementation depend on the SDIO host and the customer board.`
        },
        {
            name        : "thresholdLen",
            displayName : "Rx Treshold Length",
            default     : 4,
            longDescription : `Rx FIFO is 256 bytes, it is two times the max block length.\n
    There is a threshold IRQ on the Rx FIFO.\n
    Threshold value must be divided by 4 and the max value is 128 bytes.\n
    note: if the threshold is bigger than the expected recieved data that the IRQ
          threshold will not rise.
          The default value is 4 bytes, in this case for any recieved data the IRQ will rise.`
        }
    ],

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/sdio/SDIOWFF3.Board.c.xdt",
        boardh : "/ti/drivers/sdio/SDIO.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - UART instance to be validated
 *  param validation - object to hold detected validation issues
 *
 *  @param $super    - needed to call the generic module's functions
 */
function validate(inst, validation, $super) {
    let thresholdLenLSB = (inst.thresholdLen & 0x0003);
    let thresholdLenV   = inst.thresholdLen;
    let maxthresholdLen = 128;

    let message;

    if ($super.validate) {
        $super.validate(inst, validation);
    }

    if (thresholdLenLSB != 0x0000) {
        message = 'Rx Treshold Length must be divided by 4 bytes';
        logError(validation, inst, "thresholdLen", message);
    }

    if (maxthresholdLen < thresholdLenV) {
        message = 'The maximum Rx Treshold Length is ' +
        maxthresholdLen + ' bytes';
        logError(validation, inst, "thresholdLen", message);
    }
}

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
    let obi;

    if (inst.sdio) {
        if (inst.sdio.clkPin) {
            clk = inst.sdio.clkPin.$solution.devicePinName.replace("_", "");
        }
        if (inst.sdio.cmdPin) {
            cmd = inst.sdio.cmdPin.$solution.devicePinName.replace("_", "");
        }

        pin = "\nCLK: " + clk + "\nCMD: " + cmd;

        if (inst.sdio.data0Pin) {
            data0 = inst.sdio.data0Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA0: " + data0;
        }
        if (inst.sdio.data1Pin) {
            data1 = inst.sdio.data1Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA1: " + data1;
        }
        if (inst.sdio.data2Pin) {
            data2 = inst.sdio.data2Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA2: " + data2;
        }
        if (inst.sdio.data3Pin) {
            data3 = inst.sdio.data3Pin.$solution.devicePinName.replace("_", "");
            pin += "\nDATA3: " + data3;
        }
        if (inst.sdio.obiPin) {
            obi = inst.sdio.obiPin.$solution.devicePinName.replace("_", "");
            pin += "\nIRQ: " + obi;
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
 *  param inst      - SDIO instance
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
        interfaceNames    : ["D0"]   /* pinmux tool name */
    };

    let data1 = {
        name              : "data1Pin",  /* config script name */
        displayName       : "DATA1 Pin", /* GUI name */
        interfaceNames    : ["D1"]   /* pinmux tool name */
    };

    let data2 = {
        name              : "data2Pin",  /* config script name */
        displayName       : "DATA2 Pin", /* GUI name */
        interfaceNames    : ["D2"]   /* pinmux tool name */
    };

    let data3 = {
        name              : "data3Pin",  /* config script name */
        displayName       : "DATA3 Pin", /* GUI name */
        interfaceNames    : ["D3"]   /* pinmux tool name */
    };

    let obi = {
        name              : "obiPin",  /* config script name */
        displayName       : "OBI Pin", /* GUI name */
        interfaceNames    : ["IRQ"]   /* pinmux tool name */
    };

    let dmaRxChannel = {
        name: "dmaRxChannel",
        displayName: "Rx DMA Channel",
        description: "DMA channel used to receive data",
        interfaceNames: ["DMA_RX"]
    };

    let dmaTxChannel = {
        name: "dmaTxChannel",
        displayName: "Tx DMA Channel",
        description: "DMA channel used to send data",
        interfaceNames: ["DMA_TX"]
    };

    let resources = [];

    resources.push(clk);
    resources.push(cmd);
    resources.push(data0);
    resources.push(data1);
    resources.push(data2);
    resources.push(data3);

    /* IRQ could behandled on DATA 1 too */
    if (inst.irqSignal == 'Out-of-band') {
        resources.push(obi);
    }

    resources.push(dmaRxChannel);
    resources.push(dmaTxChannel);

    let sdio = {
        name          : "sdio",
        displayName   : "SDIO Peripheral",
        interfaceName : "SDIO",
        resources     : resources
    };

    return [sdio];
}


/*
 *  ======== extend ========
 */
 
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "SDIO", null,
        [{name: "SDIOWFF3"}], null);

    /* override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

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
    /* required function, called by base SDIO module */
    extend: extend
};
