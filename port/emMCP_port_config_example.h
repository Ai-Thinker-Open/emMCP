/**
 * @file emMCP_port_config_example.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief emMCP 端口配置示例 - STM32 HAL + FreeRTOS
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 * @note 这是一个实际可用的配置示例，适用于 STM32 HAL + FreeRTOS 环境。
 *       你可以根据自己的项目需求修改此文件。
 */

#ifndef __EMMCP_PORT_CONFIG_EXAMPLE_H__
#define __EMMCP_PORT_CONFIG_EXAMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* 头文件包含                                                                  */
/* ========================================================================== */

// STM32 HAL 头文件
#include "stm32f1xx_hal.h"  // 根据你的芯片型号修改

// FreeRTOS 头文件
#include "FreeRTOS.h"
#include "cmsis_os.h"

// 项目日志头文件
#include "log.h"

/* ========================================================================== */
/* 打印函数配置                                                               */
/* ========================================================================== */

/**
 * @brief emMCP 打印函数
 * 
 * 使用项目中定义的 log_printf 函数
 * 如果项目中没有 log_printf，可以改为 printf 或其他自定义函数
 */
#ifndef emMCP_printf
    #define emMCP_printf log_printf
#endif

/* ========================================================================== */
/* 内存管理函数配置                                                           */
/* ========================================================================== */

/**
 * @brief emMCP 内存分配和释放函数
 * 
 * 使用 FreeRTOS 的内存管理函数
 * 如果没有使用 FreeRTOS，可以使用标准C库的 malloc/free
 */
#ifndef emMCP_malloc
    #define emMCP_malloc pvPortMalloc
#endif

#ifndef emMCP_free
    #define emMCP_free vPortFree
#endif

/* ========================================================================== */
/* 延时函数配置                                                               */
/* ========================================================================== */

/**
 * @brief emMCP 延时函数
 * 
 * 使用 FreeRTOS 的延时函数
 * 如果没有使用 FreeRTOS，可以使用 HAL_Delay
 */
#ifndef emMCP_delay
    #define emMCP_delay osDelay
#endif

/* ========================================================================== */
/* 验证所有宏都已定义                                                          */
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

#endif // __EMMCP_PORT_CONFIG_EXAMPLE_H__
