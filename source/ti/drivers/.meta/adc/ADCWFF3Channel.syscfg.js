/*
 * Copyright (c) 2024-2025, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ADCWFF3Channel.syscfg.js ========
 */

"use strict";

/* Get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let config = [
    {
        name: "$name",
        defaultPrefix: "CONFIG_ADC_CHANNEL_",
        description: "C identifier used to identify this ADCBuf channel.",
        hidden: false
    },
    {
        name: "referenceSource",
        displayName: "Reference Source",
        default: "ADCWFF3_VDDA_REFERENCE",
        description: "Specifies the ADC's reference source.",
        options: [
            { displayName: "VDDA", name: "ADCWFF3_VDDA_REFERENCE", description: "VDDA must be 1.8 V" },
            { displayName: "Internal", name: "ADCWFF3_INTERNAL_REFERENCE" },
            { displayName: "External", name: "ADCWFF3_EXTERNAL_REFERENCE", description: "The VPP pin must be supplied with 1.8 V" }
        ]
    },
    {
        name: "internalSignal",
        displayName: "Internal Signal",
        description: "Specifies internal signal(s) to use as a sample source.",
        default: "None",
        options: [
            { name: "None" },
            {
                name: "Vio Channel",
                description: "Adds a channel to sample the internal VIO voltage."
            },
            {
                name: "Battery Channel",
                description: "Adds a channel to sample the internal battery voltage divided by 3, VDDA/3."
            },
            {
                name: "Vdda Channel",
                description: "Adds a channel to sample the internal VDDA voltage."
            },
            {
                name: "Temperature Channel",
                description: "Adds a channel to sample the internal temperature sensor."
            }
        ]
    }
];

/*
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  @param inst    - a fully configured ADCWFF3Channel instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{

    if (inst.internalSignal != "None") {
        return [];
    }

    let adc = {
        name: "adc",
        hidden: true,
        displayName: "ADC Peripheral",
        interfaceName: "ADC",
        canShareWith: "ADC",
        resources: [

        ],
        signalTypes: {
            adcPin: ["AIN"]
        }
    };

    let adcPinResources = [
        {
            name: "adcPin",
            canShareWith: "ADC",
            hidden: false,
            displayName: "ADC Pin",
            interfaceNames: [
                "CH0", "CH1", "CH2", "CH3",
                "CH4", "CH5", "CH6", "CH7"
            ]
        }
    ];

    adc.resources = adc.resources.concat(adcPinResources);

    return [adc];
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let modInstances = new Array();
    let shortName = inst.$name.replace("CONFIG_", "");

    if (inst.internalSignal === "None") {
        modInstances.push(
            {
                name: "adcPinInstance",
                displayName: "Input configuration when not in use",
                moduleName: "/ti/drivers/GPIO",
                collapsed: true,
                requiredArgs: {
                    parentInterfaceName: "adc",
                    parentSignalName: "adcPin",
                    parentSignalDisplayName: shortName
                },
                args: {
                    $name: "CONFIG_GPIO_" + shortName,
                    mode: "Input"
                }
            }
        );
    }

    return modInstances;
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.internalSignal === "None") {
        let devicePinName = inst.adc.adcPin.$solution.devicePinName;
        pin = "DIO" + devicePinName.substring(4);
    }
    else {
        pin = inst.internalSignal;
    }

    return pin;
}

function _getChannelInfo(inst)
{
    let result =
    {
        inputPin: "GPIO_INVALID_INDEX",
        adcChannel: 9, /* Battery channel */
        refSource: inst.referenceSource
    };

    if (inst.internalSignal === "None") {
        result.inputPin = inst.adcPinInstance.$name;

        /* Extract channel number from peripheralPinName on the form "ADC_CH2" */
        let channelName = inst.adc.adcPin.$solution.peripheralPinName.substring(6);
        let channelNum = parseInt(channelName, 10);
        if (isNaN(channelNum) || channelNum < 0 || channelNum > 7) {
            throw new Error(`Invalid ADC channel number: ${channelName}, extracted from ${inst.adc.adcPin.$solution.peripheralPinName}`);
        }
        result.adcChannel = channelNum;
    } else if(inst.internalSignal === "Vio Channel"){
        result.adcChannel = 8;
    } else if(inst.internalSignal === "Battery Channel"){
        result.adcChannel = 9;
    } else if(inst.internalSignal === "Vdda Channel"){
        result.adcChannel = 10;
    } else if(inst.internalSignal === "Temperature Channel"){
        result.adcChannel = 11;
    }

    return result;
}

/*
 *  ======== exports ========
 */
exports = {
    config: config,

    displayName: "ADC Channel",

    /* Override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    moduleInstances: moduleInstances,
    modules: Common.autoForceModules(["Board"]),

    _getPinResources: _getPinResources,

    _getChannelInfo: _getChannelInfo
};
