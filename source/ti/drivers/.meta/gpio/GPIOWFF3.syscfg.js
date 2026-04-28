/*
 * Copyright (c) 2023-2024, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== GPIOWFF3.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base GPIO configuration
 */
let devSpecific = {
    templates:
    {
        boardc : "/ti/drivers/gpio/GPIOWFF3.Board.c.xdt",
        board_initc : "/ti/drivers/gpio/GPIO.Board_init.c.xdt",
        boardh : "/ti/drivers/gpio/GPIO.Board.h.xdt"
    },

    _getPinResources: _getPinResources,
    _getDefaultName: _getDefaultName,
    _getDefaultAttrs: _getDefaultAttrs,
    _getInaccessiblePinAttrs: _getInaccessiblePinAttrs,
    _getHwSpecificAttrs: _getHwSpecificAttrs,
    _pinToDio: _pinToDio,
    _isDioAccessible: _isDioAccessible
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.gpioPin) {
        pin = inst.gpioPin.$solution.devicePinName;
        pin = pin.replace("_", "");

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += ", " + inst.$hardware.displayName;
        }
    }

    return pin;
}

/*
 *  ======== _getDefaultName ========
 */
/* istanbul ignore next */
function _getDefaultName(dioNumber)
{
    /* Some GPIOs have special names, like "LOGGER" and "SWDIO".
     * We can't generate these names based on dioNumber directly.
     * However, all GPIOs can be muxed to a Wi-Fi GPIO, and all of these are
     * named as "WI-FI_GPIO_#". There is a one to one mapping between GPIO pins
     * and Wi-Fi GPIO signals. With this knowledge, the GPIO name can be
     * extracted as below.
     */
    return system.deviceData.peripheralPins["WI-FI_GPIO_" + dioNumber].pinMappings[0].designSignalName;
}

/*
 *  ======== _getDefaultAttrs ========
 */
/* istanbul ignore next */
function _getDefaultAttrs(dioNumber)
{
    return "GPIOWFF3_DO_NOT_CONFIG";
}

/*
 *  ======== _getInaccessiblePinAttrs ========
 */
/* istanbul ignore next */
function _getInaccessiblePinAttrs()
{
    return "GPIOWFF3_DO_NOT_CONFIG";
}

/*
 *  ======== _getHwSpecificAttrs ========
 */
function _getHwSpecificAttrs(inst)
{
    return [];
}

/*
 *  ======== _pinToDio ========
 */
function _pinToDio(pinSolution, devicePin)
{
    /* Some GPIOs have special names, like "LOGGER" and "SWDIO".
     * We can't extract the DIO number directly from these names.
     * However, all GPIOs can be muxed to a Wi-Fi GPIO, and all of these are
     * named as "WI-FI_GPIO_#". There is a one to one mapping between GPIO pins
     * and Wi-Fi GPIO signals. With this knowledge, the DIO number can be
     * extracted as below.
     */

    /* Get mux settings for pin */
    let muxSettings = devicePin.mux.muxSetting;

    /* Find Wi-Fi GPIO mux setting */
    for (let muxSetting of muxSettings)
    {
        let peripheralName = muxSetting.peripheralPin.name;
        if (peripheralName.startsWith("WI-FI_GPIO_"))
        {
            let pinNumberStr = peripheralName.substring(11);
            return parseInt(pinNumberStr, 10);
        }
    }

    /* This part should never be reached */
    return -1;
}

/*
 *  ======== _isDioAccessible ========
 */
function _isDioAccessible(module, dioNumber)
{

    /* Some GPIOs have special names, like "LOGGER" and "SWDIO".
     * We can't generate these names based on dioNumber directly.
     * However, all GPIOs can be muxed to a Wi-Fi GPIO, and all of these are
     * named as "WI-FI_GPIO_#". There is a one to one mapping between GPIO pins
     * and Wi-Fi GPIO signals. With this knowledge, the GPIO pin can be
     * extracted as below.
     */
    if(system.deviceData.peripheralPins["WI-FI_GPIO_" + dioNumber] == undefined)
    {
        return false;
    }

    let pin = system.deviceData.peripheralPins["WI-FI_GPIO_" + dioNumber].pinMappings[0];

    /* If there exists a mapping between the Wi-Fi GPIO and a pin, then `pin` is
     * defined, otherwise it is undefined
     */
    if(pin === undefined)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 *  Called on instantiation and every config change.
 *
 *  $super is needed to call the generic module's functions
 */
function pinmuxRequirements(inst, $super)
{
    let result = $super.pinmuxRequirements ? $super.pinmuxRequirements(inst) : [];
    return (result);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic module to
 *  allow us to augment and override as needed.
 */
function extend(base)
{
    /* override base pinmuxRequirements */
    devSpecific.pinmuxRequirements = function (inst) {
        return pinmuxRequirements(inst, base);
    };

    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(
        base,
        "GPIO",
        null,
        [{name: "GPIOWFF3"}],
        null
    );

    /* add WFF3-supported options to generic fields */
    for (let i = 0; i < base.config.length; i++) {
        if (base.config[i].name === "interruptTrigger") {
            base.config[i].options.push({ name: "High" });
            base.config[i].options.push({ name: "Low" });
        }
    }

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    extend: extend
};
