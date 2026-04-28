cmake_minimum_required(VERSION 3.21.3)
include(${CMAKE_CURRENT_LIST_DIR}/CMakeSources_common.cmake)

set(SOURCES_CC35XX
    ${SOURCES_COMMON}
    ADC.c
    adc/ADCWFF3.c
    # adc/ADCCC32XX.c
    # Camera.c
    # camera/CameraCC32XXDMA.c
    # Capture.c
    # capture/CaptureCC32XX.c
    cryptoutils/sharedresources/CommonResourceXXF3.c
    cryptoutils/sharedresources/HSMResourceXXF3.c
    dma/DMAWFF3.c
    GPIO.c
    gpio/GPIOWFF3.c
    I2C.c
    i2c/I2CWFF3.c
    I2S.c
    i2s/I2SWFF3.c
    ITM.c
    itm/ITMWFF3.c
    # itm/ITMCC32XX.c
    # NVS.c
    xmem/XMEMWFF3.c
    xmem/flash/FlashWFF3.c
    # nvs/NVSRAM.c
    # nvs/NVSSPI25X.c
    power/PowerWFF3.c
    # power/PowerCC32XX_${TI_TOOLCHAIN_NAME}.asm
    PWM.c
    pwm/PWMTimerWFF3.c
    SD.c
    sd/SDWFF3.c
    SDIO.c
    sdio/SDIOWFF3.c
    # sd/SDSPI.c
    # SDFatFS.c
    SPI.c
    spi/SPIWFF3DMA.c
    #Timer.c
    timer/GPTimerWFF3.c
    UART2.c
    uart2/UART2WFF3.c
    Watchdog.c
    watchdog/WatchdogWFF3.c
)

set(SOURCES_SEC_ONLY_CC35XX
    ${SOURCES_CC35XX}
    # power/PowerCC26X2_calibrateRCOSC_helpers.c
    # power/PowerCC26X2_helpers.c
)
