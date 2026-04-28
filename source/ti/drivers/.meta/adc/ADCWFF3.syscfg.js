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
 *  ======== ADCWFF3.syscfg.js ========
 */

"use strict";

/* Get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ADC configuration
 */
let devSpecific = {
    config: [
        {
            name: "adcFullScaleRange",
            displayName: "ADC Full Scale Range",
            description: "Specifies the voltage range the ADC can measure.",
            default: "ADCWFF3_FULL_SCALE_RANGE_0V0_3V3",
            options: [
                { displayName: "0 V to 3.3 V", name: "ADCWFF3_FULL_SCALE_RANGE_0V0_3V3"},
                { displayName: "0.1 V to 3.3 V", name: "ADCWFF3_FULL_SCALE_RANGE_0V1_3V3"},
                { displayName: "0 V to 3.2 V", name: "ADCWFF3_FULL_SCALE_RANGE_0V0_3V2"},
                { displayName: "0 V to 1.8 V", name: "ADCWFF3_FULL_SCALE_RANGE_0V0_1V8"}
            ]
        },
        {
            name: "adcSampleClockSource",
            displayName: "ADC Sample Clock Source",
            description: "Specifies the source of the ADC sample clock",
            default: "ADCWFF3_SAMPLE_CLK_SOC_CLK",
            options: [
                { displayName: "SOC clock", name: "ADCWFF3_SAMPLE_CLK_SOC_CLK", description: "80 MHz" },
                { displayName: "HFXT clock", name: "ADCWFF3_SAMPLE_CLK_HFXT", description: "40 MHz" },
                { displayName: "SOC PLL clock", name: "ADCWFF3_SAMPLE_CLK_SOC_PLL_CLK_DIV", description: "80 MHz" }
            ]
        },
        {
            name: "adcClockDivider",
            displayName: "ADC Clock Divider",
            description: "Specifies the sample clock divider value.",
            longDescription: "This option, along with the ADC sample duration must " +
                "ensure a minimum sampling time of 500 ns if using an external reference source or 1000 ns if using " +
                "an internal reference source.",
            default: "ADCWFF3_CLKDIV_8",
            options: [
                { displayName: "1", name: "ADCWFF3_CLKDIV_1" },
                { displayName: "2", name: "ADCWFF3_CLKDIV_2" },
                { displayName: "4", name: "ADCWFF3_CLKDIV_4" },
                { displayName: "8", name: "ADCWFF3_CLKDIV_8" },
                { displayName: "16", name: "ADCWFF3_CLKDIV_16" },
                { displayName: "24", name: "ADCWFF3_CLKDIV_24" },
                { displayName: "32", name: "ADCWFF3_CLKDIV_32" },
                { displayName: "48", name: "ADCWFF3_CLKDIV_48" }
            ]
        },
        {
            name: "adcSampleTime",
            displayName: "ADC Sample Duration",
            description: "Specifies the ADC sample duration in divided sample clock cycles. A value of 0 will count as 1 cycle.",
            longDescription: "This option, along with the ADC clock divider must ensure a minimum sampling time of " +
                "500 ns if using an external reference source or 1000 ns if using an internal reference source.",
            default: 20,
            range: [0, 16383]
        }
    ],

    modules: Common.autoForceModules(["Board", "Power"]),

    moduleInstances: moduleInstances,

    templates: {
        boardc: "/ti/drivers/adc/ADCWFF3.Board.c.xdt",
        boardh: "/ti/drivers/adc/ADC.Board.h.xdt"
    }
};

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    /* This avoids constructions like CONFIG_GPIO_CONFIG_ADC_0_AIN */
    let shortName = inst.$name.replace("CONFIG_", "");
    let instances = new Array();

    instances.push(
        {
            name: "adcChannel",
            displayName: "ADC Channel",
            moduleName: "/ti/drivers/adc/ADCWFF3Channel",
            collapsed: false,
            args: {
                $name: "CONFIG_" + shortName + "_CHANNEL"
            }
        }
    );

    return instances;
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic ADC module to
 *  allow us to augment and override as needed.
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "ADC", null,
        [{ name: "ADCWFF3" }], null);

    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return result;
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADC module */
    extend: extend
};
