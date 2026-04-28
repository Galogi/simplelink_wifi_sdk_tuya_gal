/*
 * Copyright (c) 2024-2025, Texas Instruments Incorporated - https://www.ti.com
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
 *  ======== WatchdogWFF3.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let logInfo = Common.logInfo;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Watchdog configuration
 */
let devSpecific = {
    config : [
        {
            name           : "period",
            displayName    : "Period",
            description    : "Watchdog timer period in milliseconds",
            longDescription:
`The period has a resolution of 7.8125 ms, which imposes limitations on
achieving certain exact values. When a requested period cannot be precisely
implemented, the system will automatically adjust to the nearest possible value
within the resolution and report this adjusted period. For instance, if a period
of 22 ms is specified, the actual implemented period will be 23.4375 ms.`,
            default        : 1000,
            range          : [8, 65535992],
            isInteger      : true
        }
    ],
    templates : {
        boardc : "/ti/drivers/watchdog/WatchdogWFF3.Board.c.xdt",
        boardh : "/ti/drivers/watchdog/Watchdog.Board.h.xdt"
    },

    pinmuxRequirements : pinmuxRequirements,

    _getNearestPossiblePeriod: _getNearestPossiblePeriod

};

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    return ([]);
}

/*
 *  ======== _getNearestPossiblePeriod ========
 *  Adjust the input period to the nearest possible period based on the timer
 *  resolution.
 *
 *  @param inst            - Watchdog timer instance
 *
 *  @returns nearestPeriod - nearest possible period to the selected period, in
 *                           milliseconds.
 */
function _getNearestPossiblePeriod(inst)
{
    let resolution = 7.8125;
    let period     = inst.period;
    let nearestTickValue;
    let tickValue;
    let nearestPeriod;

    /* The period has a resolution of 7.8125 ms, which imposes limitations on
     * achieving certain exact values. To find the nearest possible period
     * within the resolution we first compute the desired tick value based on
     * the selected period. Then, we round this value to the nearest integer to
     * find the nearest possible tick value. Lastly, the nearest tick value is
     * used to compute the nearest possible period. The period is in
     * milliseconds.
     */
    tickValue = period / resolution;
    nearestTickValue = Math.round(tickValue);
    nearestPeriod = resolution * nearestTickValue;

    return nearestPeriod;
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - Watchdog timer instance to be validated
 *  @param validation - object to hold detected validation issues
 *
 *  @param $super     - needed to call the generic module's functions
 */
function validate(inst, validation, $super)
{
    let period     = inst.period;
    let actualPeriod;
    let message;

    if ($super.validate) {
        $super.validate(inst, validation); // Defer to the base validation
    }

    /* Report the actual implemented period when a selected period cannot be
     * precisely implemented.
     */
    actualPeriod = inst.$module._getNearestPossiblePeriod(inst);
    if (period != actualPeriod) {
        message = 'Actual period value for this instance will be: ' +
        actualPeriod + ' ms due to HW resolution.';
        logInfo(validation, inst, "period", message);
    }
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    /* Display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "Watchdog", null,
        [{name: "WatchdogWFF3"}], null);

    /* Override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* Required function, called by base Watchdog module */
    extend : extend
};
