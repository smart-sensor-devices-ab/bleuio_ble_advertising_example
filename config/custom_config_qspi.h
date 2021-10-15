/**
\addtogroup BSP
\{
\addtogroup CONFIG
\{
\addtogroup CUSTOM
\{
*/

/**
 ****************************************************************************************
 *
 * @file custom_config_qspi.h
 *
 * @brief Board Support Package. User Configuration file for cached QSPI mode.
 *
 * Copyright (C) 2015-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_QSPI_H_
#define CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE
#undef CONFIG_USE_FTDF


#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_FLASH
#define dg_configEXT_CRYSTAL_FREQ               EXT_CRYSTAL_IS_16M

/* Change dg_configUSE_LP_CLK to LP_CLK_RCX for Dongle and LP_CLK_32768 for DK */
#if defined(RELEASE_BUILD)
#define dg_configUSE_LP_CLK                     LP_CLK_RCX
#define dg_configIMAGE_SETUP                    PRODUCTION_MODE
#define dg_configSKIP_MAGIC_CHECK_AT_START      (1) // Only in PRODUCTION_MODE
#define dg_configENABLE_DEBUGGER                (0) //Set to 1 in DEVELOPMENT_MODE
#define dg_configIMAGE_FLASH_OFFSET             (0x20000)
#define USE_PARTITION_TABLE_1MB_WITH_SUOTA
#else
#define dg_configUSE_LP_CLK                     LP_CLK_32768
#define dg_configIMAGE_SETUP                    DEVELOPMENT_MODE
#endif


#define dg_configEMULATE_OTP_COPY               (0)
#define dg_configUSER_CAN_USE_TIMER1            (0)

#define dg_configUSE_WDOG                       (1)
#define dg_configWDOG_MAX_TASKS_CNT             (5)


#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8)
#define dg_configFLASH_POWER_DOWN               (0)

#define dg_configPOWER_1V8_ACTIVE               (1)
#define dg_configPOWER_1V8_SLEEP                (1)

#define dg_configBATTERY_TYPE                   (BATTERY_TYPE_NO_BATTERY)
#define dg_configBATTERY_CHARGE_CURRENT         2       // 30mA
#define dg_configBATTERY_PRECHARGE_CURRENT      20      // 2.1mA
#define dg_configBATTERY_CHARGE_NTC             1       // disabled

#define dg_configUSE_USB                        1
#define dg_configUSE_USB_CHARGER                0
#define dg_configALLOW_CHARGING_NOT_ENUM        0

#define dg_configUSE_ProDK                      (1)

#define dg_configUSE_SW_CURSOR                  (0)

#define dg_configCACHEABLE_QSPI_AREA_LEN        (NVMS_PARAM_PART_start - MEMORY_QSPIF_BASE)

/*************************************************************************************************\
 * Memory layout specific config
 */
#define dg_configQSPI_CACHED_RAM_SIZE           (56 * 1024)
#define dg_configQSPI_CACHED_RETRAM_0_SIZE      (72 * 1024)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    18432   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */
#define dg_configUSE_USB_ENUMERATION            1

#define dg_configUSE_HW_USB                     1
#define dg_configFLASH_ADAPTER                  1
#define dg_configNVMS_ADAPTER                   1
#define dg_configNVMS_VES                       0
#define dg_configGPADC_ADAPTER                  0


/*************************************************************************************************\
 * BLE device config
 */
#define dg_configBLE_CENTRAL                    (0)
#define dg_configBLE_GATT_CLIENT                (0)
#define dg_configBLE_OBSERVER                   (0)
#define dg_configBLE_BROADCASTER                (0)
#define dg_configBLE_L2CAP_COC                  (0)

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include memory layout */
#include "bsp_memory_layout.h"

#endif /* CUSTOM_CONFIG_QSPI_H_ */

/**
\}
\}
\}
*/
