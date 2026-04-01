/**
 * @file emMCP_port_config_stm32.h
 * @brief STM32 HAL platform configuration for emMCP
 * @version 1.0.0
 * @date 2025-09-30
 */

#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* 平台检测                                                                    */
/* ========================================================================== */

#if defined(__STM32_HAL_H) || defined(__STM32F1xx_HAL_H) || defined(__STM32F4xx_HAL_H)
    #ifndef EMCP_PLATFORM_STM32_HAL
        #define EMCP_PLATFORM_STM32_HAL
    #endif
#endif

#if defined(FREERTOS_H) || defined(__FREERTOS__)
    #ifndef EMCP_PLATFORM_FREERTOS
        #define EMCP_PLATFORM_FREERTOS
    #endif
#endif

/* ========================================================================== */
/* 头文件包含                                                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>

#ifdef EMCP_PLATFORM_FREERTOS
    #include "FreeRTOS.h"
    #include "cmsis_os.h"
#endif

#ifdef EMCP_PLATFORM_STM32_HAL
    #include "stm32f1xx_hal.h"  // 根据你的STM32系列修改
#endif

/* ========================================================================== */
/* 打印函数配置                                                                */
/* ========================================================================== */

#ifndef emMCP_printf
    #define emMCP_printf printf
#endif

/* ========================================================================== */
/* 内存管理函数配置                                                            */
/* ========================================================================== */

#ifndef emMCP_malloc
    #ifdef EMCP_PLATFORM_FREERTOS
        #define emMCP_malloc pvPortMalloc
    #else
        #define emMCP_malloc malloc
    #endif
#endif

#ifndef emMCP_free
    #ifdef EMCP_PLATFORM_FREERTOS
        #define emMCP_free vPortFree
    #else
        #define emMCP_free free
    #endif
#endif

/* ========================================================================== */
/* 延时函数配置                                                                */
/* ========================================================================== */

#ifndef emMCP_delay
    #ifdef EMCP_PLATFORM_STM32_HAL
        #define emMCP_delay HAL_Delay
    #elif defined(EMCP_PLATFORM_FREERTOS)
        #define emMCP_delay osDelay
    #else
        #error "emMCP_delay must be defined"
    #endif
#endif

/* ========================================================================== */
/* 配置验证                                                                    */
/* ========================================================================== */

#ifndef emMCP_printf
    #error "emMCP_printf is not defined"
#endif
#ifndef emMCP_malloc
    #error "emMCP_malloc is not defined"
#endif
#ifndef emMCP_free
    #error "emMCP_free is not defined"
#endif
#ifndef emMCP_delay
    #error "emMCP_delay is not defined"
#endif

#ifdef __cplusplus
}
#endif

#endif // __EMMCP_PORT_CONFIG_H__
