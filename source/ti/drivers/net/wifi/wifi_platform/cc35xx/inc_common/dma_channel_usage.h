/*
 * Copyright (c) 2024, Texas Instruments Incorporated
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
#ifndef LOWER_MAC_DMA_CHANNEL_USAGE_H_
#define LOWER_MAC_DMA_CHANNEL_USAGE_H_

typedef enum {
    HOSTDMA_DRIVER_CH_UARTRX_IN       = 0,
    HOSTDMA_DRIVER_CH_UARTTX_OUT      = 1,
    HOSTDMA_DRIVER_CH_UARTRX_IN_1       = 2,
    HOSTDMA_DRIVER_CH_UARTTX_OUT_1      = 3,
    HOSTDMA_DRIVER_CH_SPIRX_IN        = 4,
    HOSTDMA_DRIVER_CH_SPITX_OUT       = 5,
    HOSTDMA_DRIVER_CH_TEMP_IN         = 6,
    HOSTDMA_DRIVER_CH_TEMP_OUT        = 7,//not valid
    HOSTDMA_DRIVER_CH_TEMP_IN1        = 8,
    HOSTDMA_DRIVER_CH_TEMP_OUT1       = 9,
    HOSTDMA_DRIVER_CH_TEMP_IN2       = 10,
    HOSTDMA_DRIVER_CH_HIF            = 11 //used by the upper mac for transport layer
} CC35xx_DMA_CHANNEL_USAGE_e;

#endif
