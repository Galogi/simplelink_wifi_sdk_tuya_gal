/*
 *  ======== ti_drivers_config.c ========
 *  Configured TI-Drivers module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_EM_CC35X1
 *  by the SysConfig tool.
 */

#include <stddef.h>
#include <stdint.h>

#ifndef DeviceFamily_CC35XX
#define DeviceFamily_CC35XX
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"


/*
 *  =============================== Key Store ===============================
 */
#include <third_party/hsmddk/include/Integration/Adapter_PSA/incl/adapter_psa_key_management.h>

uint8_t volatileAllocBuffer[KEYSTORE_VOLATILE_MEMORY_POOL_SIZE];
const size_t volatileAllocBufferSizeBytes  = KEYSTORE_VOLATILE_MEMORY_POOL_SIZE;

const size_t MBEDTLS_KEY_VOLATILE_COUNT    = KEYSTORE_VOLATILE_SLOT_COUNT;
const size_t MBEDTLS_KEY_ASSET_STORE_COUNT = KEYSTORE_ASSET_STORE_SLOT_COUNT;
/* For cache slots */
const size_t MBEDTLS_KEY_PERSISTENT_COUNT  = KEYSTORE_PERSISTENT_SLOT_COUNT;
/* For Key Store flash space */
const size_t FLASH_KEY_PERSISTENT_COUNT    = KEYSTORE_PERSISTENT_NUM_KEYS;

psa_key_context_t gl_PSA_Key[KEYSTORE_TOTAL_SLOT_COUNT];


/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>

/* The range of pins available on this device */
const uint_least8_t GPIO_pinLowerBound = 0;
const uint_least8_t GPIO_pinUpperBound = 37;

/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[GPIO_NUMBER_OF_CONFIGS] = {
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO0 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO1 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO2 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO3 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO4 */
    /* Owned by CONFIG_UART2_0 as TX */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH, /* CONFIG_GPIO_UART2_0_TX */
    /* Owned by CONFIG_UART2_0 as RX */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_DOWN_INTERNAL, /* CONFIG_GPIO_UART2_0_RX */
    GPIOWFF3_DO_NOT_CONFIG, /* Pin is not available on this device */
    GPIOWFF3_DO_NOT_CONFIG, /* Pin is not available on this device */
    GPIOWFF3_DO_NOT_CONFIG, /* Pin is not available on this device */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO10 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO11 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO12 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO13 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO14 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO15 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO16 */
    /* Owned by CONFIG_UART2_1 as TX */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH, /* CONFIG_GPIO_UART2_1_TX */
    /* Owned by CONFIG_UART2_1 as RX */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_DOWN_INTERNAL, /* CONFIG_GPIO_UART2_1_RX */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO19 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO20 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO21 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO22 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO23 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO24 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO25 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO26 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO27 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO28 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO29 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO30 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO31 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO32 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO33 */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW, /* CONFIG_GPIO_LED_0 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO35 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO36 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO37 */
};

/*
 *  ======== gpioCallbackFunctions ========
 *  Array of callback function pointers
 *  Change at runtime with GPIO_setCallback()
 */
GPIO_CallbackFxn gpioCallbackFunctions[GPIO_NUMBER_OF_CONFIGS];

/*
 *  ======== gpioUserArgs ========
 *  Array of user argument pointers
 *  Change at runtime with GPIO_setUserArg()
 *  Get values with GPIO_getUserArg()
 */
void* gpioUserArgs[GPIO_NUMBER_OF_CONFIGS];

const uint_least8_t CONFIG_GPIO_LED_0_CONST = CONFIG_GPIO_LED_0;
const uint_least8_t CONFIG_GPIO_UART2_0_TX_CONST = CONFIG_GPIO_UART2_0_TX;
const uint_least8_t CONFIG_GPIO_UART2_0_RX_CONST = CONFIG_GPIO_UART2_0_RX;
const uint_least8_t CONFIG_GPIO_UART2_1_TX_CONST = CONFIG_GPIO_UART2_1_TX;
const uint_least8_t CONFIG_GPIO_UART2_1_RX_CONST = CONFIG_GPIO_UART2_1_RX;

/*
 *  ======== GPIO_config ========
 */
const GPIO_Config GPIO_config = {
    .configs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .userArgs = gpioUserArgs,
    .intPriority = (~0)
};

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>

extern void PowerWFF3_sleepPolicy(void);
/*
 *  This structure defines the configuration for the Power Manager.
 */
const PowerWFF3_Config PowerWFF3_config = {
    .policyInitFxn             = NULL,
    .policyFxn                 = PowerWFF3_sleepPolicy,
};

/*
 *  =============================== RNG ===============================
 */

#include <ti/drivers/RNG.h>
#include <ti/drivers/rng/RNGXXF3HSM.h>

#define CONFIG_RNG_COUNT 0

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=32
#else
__attribute__ ((aligned (32)))
#endif

const RNG_ReturnBehavior RNGXXF3HSM_returnBehavior = RNG_RETURN_BEHAVIOR_POLLING;

RNGXXF3HSM_Object RNGXXF3HSM_objects[CONFIG_RNG_COUNT];

/*
 *  ======== RNGXXF3HSMHWAttrs ========
 */
const RNGXXF3HSM_HWAttrs RNGXXF3HSM_hwAttrs[CONFIG_RNG_COUNT] = {
};

const RNG_Config RNG_config[CONFIG_RNG_COUNT] = {
};


const uint_least8_t RNG_count = CONFIG_RNG_COUNT;

/*
 *  =============================== UART2 ===============================
 */

#include <ti/drivers/UART2.h>
#include <ti/drivers/uart2/UART2WFF3.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/dma/DMAWFF3.h>
#include <ti/drivers/Power.h>
#include <ti/devices/cc35xx/inc/hw_memmap.h>
#include <ti/devices/cc35xx/inc/hw_ints.h>
#include <ti/devices/cc35xx/inc/hw_soc_aon.h>

#define CONFIG_UART2_COUNT 2

UART2WFF3_Object UART2WFF3_objects[CONFIG_UART2_COUNT];

static unsigned char uart2RxRingBuffer0[32];
/* TX ring buffer allocated to be used for nonblocking mode */
static unsigned char uart2TxRingBuffer0[32];
static unsigned char uart2RxRingBuffer1[32];
/* TX ring buffer allocated to be used for nonblocking mode */
static unsigned char uart2TxRingBuffer1[32];

static const UART2WFF3_HWAttrs UART2WFF3_hwAttrs[CONFIG_UART2_COUNT] = {
  {
    .baseAddr           = UARTLIN1_BASE,
    .intNum             = INT_SP_UART_1_INT_REQ,
    .intPriority        = (~0),
    .rxPin              = CONFIG_GPIO_UART2_0_RX,
    .txPin              = CONFIG_GPIO_UART2_0_TX,
    .ctsPin             = GPIO_INVALID_INDEX,
    .rtsPin             = GPIO_INVALID_INDEX,
    .flowControl        = UART2_FLOWCTRL_NONE,
    .rxBufPtr           = uart2RxRingBuffer0,
    .rxBufSize          = sizeof(uart2RxRingBuffer0),
    .txBufPtr           = uart2TxRingBuffer0,
    .txBufSize          = sizeof(uart2TxRingBuffer0),
    .txPinMux           = 5,
    .rxPinMux           = 5,
    .ctsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rtsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rxDmaChannel       = 3,
    .txDmaChannel       = 2,
    .codingScheme       = UART2WFF3_CODING_UART,
    .concatenateFIFO    = false,
    .irLPClkDivider     = 26,
    .powerID            = PowerWFF3_PERIPH_UARTLIN1
  },
  {
    .baseAddr           = UARTLIN0_BASE,
    .intNum             = INT_SP_UART_0_INT_REQ,
    .intPriority        = (~0),
    .rxPin              = CONFIG_GPIO_UART2_1_RX,
    .txPin              = CONFIG_GPIO_UART2_1_TX,
    .ctsPin             = GPIO_INVALID_INDEX,
    .rtsPin             = GPIO_INVALID_INDEX,
    .flowControl        = UART2_FLOWCTRL_NONE,
    .rxBufPtr           = uart2RxRingBuffer1,
    .rxBufSize          = sizeof(uart2RxRingBuffer1),
    .txBufPtr           = uart2TxRingBuffer1,
    .txBufSize          = sizeof(uart2TxRingBuffer1),
    .txPinMux           = 5,
    .rxPinMux           = 5,
    .ctsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rtsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rxDmaChannel       = 0,
    .txDmaChannel       = 1,
    .codingScheme       = UART2WFF3_CODING_UART,
    .concatenateFIFO    = false,
    .irLPClkDivider     = 26,
    .powerID            = PowerWFF3_PERIPH_UARTLIN0
  },
};

const UART2_Config UART2_config[CONFIG_UART2_COUNT] = {
    {   /* CONFIG_UART2_0 */
        .object      = &UART2WFF3_objects[CONFIG_UART2_0],
        .hwAttrs     = &UART2WFF3_hwAttrs[CONFIG_UART2_0]
    },
    {   /* CONFIG_UART2_1 */
        .object      = &UART2WFF3_objects[CONFIG_UART2_1],
        .hwAttrs     = &UART2WFF3_hwAttrs[CONFIG_UART2_1]
    },
};

const uint_least8_t CONFIG_UART2_0_CONST = CONFIG_UART2_0;
const uint_least8_t CONFIG_UART2_1_CONST = CONFIG_UART2_1;
const uint_least8_t UART2_count = CONFIG_UART2_COUNT;


#include <stdbool.h>

#include <ti/drivers/GPIO.h>



#include <ti/drivers/Board.h>

/*
 *  ======== Board_initHook ========
 *  Perform any board-specific initialization needed at startup.  This
 *  function is declared weak to allow applications to override it if needed.
 */
void __attribute__((weak)) Board_initHook(void)
{
}

/*
 *  ======== Board_init ========
 *  Perform any initialization needed before using any board APIs
 */
void Board_init(void)
{
    /* ==== /ti/drivers/Power initialization ==== */
    Power_init();

    /* ==== /ti/drivers/GPIO initialization ==== */
    /* Setup GPIO module and default-initialise pins */
    GPIO_init();

    /* ==== /ti/drivers/DMA initialization ==== */
    DMAWFF3_init();


    Board_initHook();
}

