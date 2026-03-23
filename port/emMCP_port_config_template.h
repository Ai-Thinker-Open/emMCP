/**
 * @file emMCP_port_config_template.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief emMCP 端口配置模板
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 * @note 这是一个模板文件，展示了如何配置 emMCP 的端口宏定义。
 *       请复制此文件到你的项目目录中，重命名为 emMCP_port_config.h，
 *       并根据你的项目需求修改以下宏定义。
 */

#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* 平台检测宏定义                                                              */
/* ========================================================================== */

/**
 * @brief STM32 HAL 平台检测
 * 如果定义了 __STM32_HAL_H，则认为是 STM32 HAL 平台
 */
#if defined(__STM32_HAL_H) || defined(__STM32F1xx_HAL_H) || defined(__STM32F4xx_HAL_H)
    #ifndef EMCP_PLATFORM_STM32_HAL
        #define EMCP_PLATFORM_STM32_HAL
    #endif
#endif

/**
 * @brief FreeRTOS 平台检测
 * 如果定义了 FREERTOS_H，则认为是 FreeRTOS 平台
 */
#if defined(FREERTOS_H) || defined(__FREERTOS__)
    #ifndef EMCP_PLATFORM_FREERTOS
        #define EMCP_PLATFORM_FREERTOS
    #endif
#endif

/* ========================================================================== */
/* 打印函数配置                                                               */
/* ========================================================================== */

/**
 * @brief 定义 emMCP 打印函数
 * 
 * 推荐配置：
 * - STM32 HAL + printf:       #define emMCP_printf printf
 * - STM32 HAL + 自定义log:    #define emMCP_printf log_printf
 * - FreeRTOS:                  #define emMCP_printf printf
 * - 无操作系统:                 根据需要自定义
 */
#ifndef emMCP_printf
    #ifdef EMCP_PLATFORM_STM32_HAL
        #include <stdio.h>
        #define emMCP_printf printf
    #else
        // 默认使用 printf
        #include <stdio.h>
        #define emMCP_printf printf
    #endif
#endif

/* ========================================================================== */
/* 内存管理函数配置                                                           */
/* ========================================================================== */

/**
 * @brief 定义 emMCP 内存分配和释放函数
 * 
 * 推荐配置：
 * - 标准C库:                    #define emMCP_malloc malloc
 *                               #define emMCP_free free
 * - FreeRTOS:                  #define emMCP_malloc pvPortMalloc
 *                               #define emMCP_free vPortFree
 * - RT-Thread:                 #define emMCP_malloc rt_malloc
 *                               #define emMCP_free rt_free
 * - 裸机/自定义:                根据你的内存管理器自定义
 */
#ifndef emMCP_malloc
    #ifdef EMCP_PLATFORM_FREERTOS
        #include "FreeRTOS.h"
        #define emMCP_malloc pvPortMalloc
    #else
        // 默认使用标准C库的 malloc
        #include <stdlib.h>
        #define emMCP_malloc malloc
    #endif
#endif

#ifndef emMCP_free
    #ifdef EMCP_PLATFORM_FREERTOS
        #include "FreeRTOS.h"
        #define emMCP_free vPortFree
    #else
        // 默认使用标准C库的 free
        #include <stdlib.h>
        #define emMCP_free free
    #endif
#endif

/* ========================================================================== */
/* 延时函数配置                                                               */
/* ========================================================================== */

/**
 * @brief 定义 emMCP 延时函数
 * 
 * 推荐配置：
 * - STM32 HAL:                 #define emMCP_delay HAL_Delay
 * - FreeRTOS:                  #define emMCP_delay osDelay
 * - RT-Thread:                 #define emMCP_delay rt_thread_mdelay
 * - 裸机/自定义:                根据你的延时函数自定义
 * 
 * @note 延时函数接收一个参数：延时时间（毫秒）
 */
#ifndef emMCP_delay
    #ifdef EMCP_PLATFORM_STM32_HAL
        #define emMCP_delay HAL_Delay
    #elif defined(EMCP_PLATFORM_FREERTOS)
        #include "cmsis_os.h"
        #define emMCP_delay osDelay
    #else
        // 对于其他平台，延时函数必须由用户定义
        #error "emMCP_delay must be defined. Please define emMCP_delay before including this header or define EMCP_PLATFORM_* to use a default implementation."
    #endif
#endif

/* ========================================================================== */
/* 配置验证                                                                   */
/* ========================================================================== */

/**
 * @brief 验证所有必需的宏都已定义
 */
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
