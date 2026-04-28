/*
 * Copyright (c) 2024, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== GPTimer.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "GPTimer");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.description = "Timer Interrupt Priority";


let timerConfig = [
    intPriority
];

/*
 *  ======== base ========
 *  Define the base GPTimer properties and methods
 */
let base = {
    displayName         : "GPTimer",
    description         : "GPTimer Driver",
    longDescription     : `
The [__Timer driver__][1] allows you to manage a GPTimer peripheral via simple
and portable APIs.

* [Peripheral properties][2]
* [Example][3]

[1]: /drivers/doxygen/html/_g_p_timer_w_f_f3_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_g_p_timer_w_f_f3_8h.html#ti_drivers_GPTimerWFF3_PeripheralProperties "Peripheral properties"
[3]: /drivers/doxygen/html/_g_p_timer_w_f_f3_8h.html#ti_drivers_GPTimerWFF3_Examples "C usage examples"
`,
    defaultInstanceName : "CONFIG_GPTIMER_",
    config              : Common.addNameConfig(timerConfig, "/ti/drivers/gptimer", "CONFIG_GPTIMER_"),
    modules             : Common.autoForceModules(["Board", "Power"])
};

/* get family-specific GPTimer module */
let devTimer = system.getScript("/ti/drivers/gptimer/GPTimer" + family);

exports = devTimer.extend(base);
