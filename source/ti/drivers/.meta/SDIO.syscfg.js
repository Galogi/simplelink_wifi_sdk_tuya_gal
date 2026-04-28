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
 *  ======== SDIO.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "SDIO");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.description = "SDIO Interrupt Priority";

/* Array of SDIO configurables that are common across device families */
let config = [
    intPriority
];

/*
 *  ======== base ========
 *  Define the base SDIO properties and methods
 */
let base = {
    displayName   : "SDIO",
    description   : "Secure Digital Input Output (SDIO) Driver",

    longDescription: `
The [__SDIO driver__][1] provides device independent APIs for I/F between hosts. 

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_s_d_i_o_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_s_d_i_o_8h.html#ti_drivers_SDIO_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_s_d_i_o_8h.html#ti_drivers_SDIO_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#SDIO_Configuration_Options "Configuration options reference"
`,
    /* instance properties and methods */
    defaultInstanceName: "CONFIG_SDIO_",
    config        : Common.addNameConfig(config, "/ti/drivers/SDIO", "CONFIG_SDIO_"),
    modules       : Common.autoForceModules(["Board", "Power", "DMA"])
};

/* get family-specific SDIO module */
let devSDIO = system.getScript("/ti/drivers/sdio/SDIO" + family);

exports     = devSDIO.extend(base);
