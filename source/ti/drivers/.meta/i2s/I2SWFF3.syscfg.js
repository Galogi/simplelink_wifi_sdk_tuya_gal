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
 *
 */

/*
 *  ======== I2SWFF3.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base I2S configuration
 */
let devSpecific = {
    config: [
        {
            name        : "enableCCLK",
            displayName : "Enable CCLK",
            default     : false
        },
        {
            displayName: "Clock Options",
            config: [
                {
                    name: "clkSrc",
                    displayName: "Clock Source",
                    description: "Clock source for the ADFS",
                    longDescription: `This option selects the input clock to the
ADFS. This clock is used to generate the audio clock (ACLK).
For details on which clock source to select, please refer to
[Selecting the Clock Source](drivers/doxygen/html/_i2_s_w_f_f3_8h.html#ti_drivers_i2s_I2SWFF3_CLK_SRC).
`,
                    options: [
                        { name: "I2SWFF3_CLK_SRC_SOC_CLK", displayName: "SOC clock", description: "80 MHz" },
                        { name: "I2SWFF3_CLK_SRC_SOC_PLL_CLK", displayName: "SOC PLL clock", description: "160 MHz" },
                        { name: "I2SWFF3_CLK_SRC_HFXT_CLK", displayName: "HFXT clock", description: "52 MHz" }
                    ],
                    default: "I2SWFF3_CLK_SRC_SOC_PLL_CLK"
                },
                {
                    name: "audioClkFreq",
                    displayName: "Audio Clock Frequency",
                    description: "Audio clock frequency",
                    longDescription: `This option selects the audio clock (ACLK)
frequency. For details on which frequency to select, please
refer to [Selecting the Audio Clock Frequency](drivers/doxygen/html/_i2_s_w_f_f3_8h.html#ti_drivers_i2s_I2SWFF3_ACLK).
`,
                    options: [
                        {
                            name: "I2SWFF3_ACLK_11P2896MHZ",
                            displayName: "11.2896 MHz",
                            description: "Suitable for sample rates of 22.05 kHz and 44.1 kHz."
                        },
                        {
                            name: "I2SWFF3_ACLK_12P288MHZ",
                            displayName: "12.288 MHz",
                            description: "Suitable for sample rates of 8 kHz, 16 kHz, 24 kHz, 32 kHz and 48 kHz."
                        },
                        {
                            name: "I2SWFF3_ACLK_CLK_SRC_FREQ",
                            displayName: "Clock source frequency",
                            description: "Not intended for any standard sample rate, but it can still be selected if desired."
                        }
                    ],
                    default: "I2SWFF3_ACLK_12P288MHZ"
                }
            ]
        }
    ],

    templates: {
        boardc : "/ti/drivers/i2s/I2SWFF3.Board.c.xdt",
        boardh : "/ti/drivers/i2s/I2S.Board.h.xdt"
    },

    pinmuxRequirements: pinmuxRequirements,
    moduleInstances: moduleInstances,
    modules: Common.autoForceModules(["Board", "Power"]),

    _getPinResources: _getPinResources,
    _getAdfsParameters: _getAdfsParameters,
    _getAudioClockFrequency: _getAudioClockFrequency
};

/*
 *  ======== _getAdfsParameters ========
 * The ADFS is used to generate an arbitrary clock frequency (Freq) from a given
 * input clock (Fref). The ADFS is configured with the following parameters:
 * - tref: Freq period in picoseconds, represented as a fixed-point number with
 *   17 integer bits and 4 fractional bits.
 * - div: Divisor used to approximate the ratio between Fref and Freq. It is
 *   calculated to minimize the error (delta) between the actual and desired
 *   frequency ratios. Should be an integer higher or equal than 1.
 * - delta: Error in picoseconds that quantifies how far the achieved period
 *   (using the chosen divider) deviates from the desired period. It is
 *   represented as a fixed-point number with 13 integer bits and 4 fractional
 *   bits.
 * - deltaSign: Represents whether delta is negative (1) or non-negative (0).
 *
 * This function returns a dictionary with the calculated parameters needed to
 * configure the ADFS to achieve the audio frequency based on the selected
 * clock source. It also returns the actual output frequency of the ADFS after
 * taking into account the rounding errors.
 */
function _getAdfsParameters(inst)
{
    let tref;
    let div_temp1, div_temp2, div;
    let delta, delta_temp1, delta_temp2;
    let deltaSign;
    let DT_MAX = 0x1FFFF;    // Delta is a 13-bit number
    let adfsOutputFreq;

    /* Get frequencies in hertz */
    let Freq = _getAudioClockFrequency(inst);
    let Fref = _getSourceClockFrequency(inst.clkSrc);

    if (Freq > Fref) {
        throw new Error("The requested frequency can't be higher than the reference frequency.");
    }

    /* Calculate tref in ps and represent as a fixed-point number with 4
     * fractional bits.
     */
    tref = Math.pow(10, 12) / Fref;
    tref = Math.floor(tref*16);

    /* Compute the divider and the delta. Since the divider must be an integer,
     * we must find which divider produces the smallest delta. The delta is
     * computed in picoseconds.
     */
    div_temp1 = Math.floor(Fref / Freq);
    div_temp2 = Math.ceil(Fref / Freq);

    delta_temp1 = Math.pow(10, 12) * ((1/(Freq*div_temp1))-(1/Fref));
    delta_temp2 = Math.pow(10, 12) * ((1/(Freq*div_temp2))-(1/Fref));

    if (Math.abs(delta_temp1) < Math.abs(delta_temp2))
    {
        div = div_temp1;
        delta = delta_temp1;
    }
    else
    {
        div = div_temp2;
        delta = delta_temp2;
    }

    /* Get the delta sign */
    deltaSign = (delta < 0) ? 1 : 0;

    /* Represent delta as a fixed-point number with 4 fractional bits */
    delta = Math.abs(delta)*16;
    delta = Math.round(delta);

    /* If the delta is overflow, use max value */
    if (delta > DT_MAX)
    {
        delta = DT_MAX;
    }

    /* Compute the actual output frequency accounting for rounding errors */
    if (deltaSign == 0) {
        adfsOutputFreq = 1 / (((Math.pow(10, 12) / Fref) + (delta/16)) * div);
    }
    else {
        adfsOutputFreq = 1 / (((Math.pow(10, 12) / Fref) - (delta/16)) * div);
    }

    adfsOutputFreq = Math.floor(Math.pow(10, 12) * adfsOutputFreq);

    return {
        tref: tref,
        div: div,
        delta: delta,
        deltaSign: deltaSign,
        adfsOutputFreq: adfsOutputFreq
    };
}

/*
 *  ======== _getAudioClockFrequency ========
 */
function _getAudioClockFrequency(inst)
{

    if (inst.audioClkFreq == "I2SWFF3_ACLK_CLK_SRC_FREQ") {
        return _getSourceClockFrequency(inst.clkSrc);
    }
    else if (inst.audioClkFreq == "I2SWFF3_ACLK_11P2896MHZ") {
        return 11289600;
    }
    else if (inst.audioClkFreq == "I2SWFF3_ACLK_12P288MHZ") {
        return 12288000;
    } else {
        throw new Error("Invalid audio clock frequency");
    }
}

/*
 *  ======== _getSourceClockFrequency ========
 */
function _getSourceClockFrequency(clockSrc)
{

    let clockFrequency = 0;

    if (clockSrc == "I2SWFF3_CLK_SRC_SOC_CLK") {
        clockFrequency = 80000000;
    }
    else if (clockSrc == "I2SWFF3_CLK_SRC_SOC_PLL_CLK") {
        clockFrequency = 160000000;
    }
    else if (clockSrc == "I2SWFF3_CLK_SRC_HFXT_CLK") {
        clockFrequency = 52000000;
    } else {
        throw new Error("Invalid clock source selected");
    }

    return (clockFrequency);
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let sckPin;
    let cclkPin;
    let wsPin;
    let sd0Pin;
    let sd1Pin;

    if (inst.i2s) {

        if (inst.i2s.SCKPin) {
            sckPin = inst.i2s.SCKPin.$solution.devicePinName.replace("_", "");
            pin = "\nSCK: " + sckPin;
        }

        if (inst.i2s.CCLKPin) {
            cclkPin = inst.i2s.CCLKPin.$solution.devicePinName.replace("_", "");
            pin += "\nCCLK: " + cclkPin;
        }

        if (inst.i2s.WSPin) {
            wsPin = inst.i2s.WSPin.$solution.devicePinName.replace("_", "");
            pin += "\nWS: " + wsPin;
        }

        if (inst.i2s.SD0Pin) {
            sd0Pin = inst.i2s.SD0Pin.$solution.devicePinName.replace("_", "");
            pin += "\nSD0: " + sd0Pin;
        }

        if (inst.i2s.SD1Pin) {
            sd1Pin = inst.i2s.SD1Pin.$solution.devicePinName.replace("_", "");
            pin += "\nSD1: " + sd1Pin;
        }

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== moduleInstances ========
 *  returns PIN instances
 */
function moduleInstances(inst)
{
    let pinInstances = new Array();
    let shortName = inst.$name.replace("CONFIG_", "");

    if (inst.sd0DataDirection != "Disabled") {
        pinInstances.push({
            name: "sd0PinInstance",
            displayName: "SD0 configuration when not in use",
            moduleName: "/ti/drivers/GPIO",
            collapsed: true,
            requiredArgs: {
                parentInterfaceName: "i2s",
                parentSignalName: "SD0Pin",
                parentSignalDisplayName: "SD0"
            },
            args: {
                $name: "CONFIG_GPIO_" + shortName + "_SD0",
                mode: "Input",
                pull: "Pull Down"
            }
        });
    }

    if (inst.sd1DataDirection != "Disabled") {
        pinInstances.push({
            name: "sd1PinInstance",
            displayName: "SD1 configuration when not in use",
            moduleName: "/ti/drivers/GPIO",
            collapsed: true,
            requiredArgs: {
                parentInterfaceName: "i2s",
                parentSignalName: "SD1Pin",
                parentSignalDisplayName: "SD1"
            },
            args: {
                $name: "CONFIG_GPIO_" + shortName + "_SD1",
                mode: "Input",
                pull: "Pull Down"
            }
        });
    }

    pinInstances.push({
        name: "sckPinInstance",
        displayName: "SCK configuration when not in use",
        moduleName: "/ti/drivers/GPIO",
        collapsed: true,
        requiredArgs: {
            parentInterfaceName: "i2s",
            parentSignalName: "SCKPin",
            parentSignalDisplayName: "SCK"
        },
        args: {
            $name: "CONFIG_GPIO_" + shortName + "_SCK",
            mode: "Input",
            pull: "Pull Down"
        }
    });

    pinInstances.push({
        name: "wsPinInstance",
        displayName: "WS configuration when not in use",
        moduleName: "/ti/drivers/GPIO",
        collapsed: true,
        requiredArgs: {
            parentInterfaceName: "i2s",
            parentSignalName: "WSPin",
            parentSignalDisplayName: "WS"
        },
        args: {
            $name: "CONFIG_GPIO_" + shortName + "_WS",
            mode: "Input",
            pull: "Pull Down"
        }
    });

    if (inst.enableCCLK) {
        pinInstances.push({
            name: "cclkPinInstance",
            displayName: "CCLK configuration when not in use",
            moduleName: "/ti/drivers/GPIO",
            collapsed: true,
            requiredArgs: {
                parentInterfaceName: "i2s",
                parentSignalName: "CCLKPin",
                parentSignalDisplayName: "CCLK"
            },
            args: {
                $name: "CONFIG_GPIO_" + shortName + "_CCLK",
                mode: "Input",
                pull: "Pull Down"
            }
        });
    }

    return (pinInstances);
}

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{

    let sd0 = {
        name: "SD0Pin",
        displayName: "SD0 Pin",
        interfaceNames: ["DATA0"]
    };

    let sd1 = {
        name: "SD1Pin",
        displayName: "SD1 Pin",
        interfaceNames: ["DATA1"]
    };

    let sck = {
        name: "SCKPin",
        displayName: "SCK Pin",
        interfaceNames: ["BCLK"]
    };

    let ws = {
        name: "WSPin",
        displayName: "WS Pin",
        interfaceNames: ["WCLK"]
    };

    let cclk = {
        name: "CCLKPin",
        displayName: "CCLK Pin",
        interfaceNames: ["MCLK"]
    };

    let resources = [];

    if (inst.sd0DataDirection != "Disabled") {
        resources.push(sd0);
    }

    if (inst.sd1DataDirection != "Disabled") {
        resources.push(sd1);
    }

    resources.push(sck);
    resources.push(ws);

    if (inst.enableCCLK == true) {
        resources.push(cclk);
    }

    let i2s = {
        name: "i2s",
        displayName: "I2S Peripheral",
        description: "I2S Multichannel Audio Serial Port",
        interfaceName: "I2S",
        resources     : resources,
        signalTypes   : {
            SD0Pin     : ['I2S_SD0'],
            SD1Pin     : ['I2S_SD1'],
            SCKPin     : ['I2S_SCK'],
            WSPin      : ['I2S_WS'],
            CCLKPin    : ['I2S_CCLK']
        }
    };

    return ([i2s]);
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - I2S instance to be validated
 *  param validation - object to hold detected validation issues
 *
 *  @param $super    - needed to call the generic module's functions
 */
function validate(inst, validation, $super)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "I2S", null,
        [{name: "I2SWFF3"}], null);

    /* override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

    /* overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 */
exports = {
    extend: extend
};
