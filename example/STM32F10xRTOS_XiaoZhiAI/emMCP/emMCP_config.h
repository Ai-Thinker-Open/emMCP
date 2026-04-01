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

#ifndef __EMMCP_CONFIG_H__
#define __EMMCP_CONFIG_H__


#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* 头文件包含                                                                  */
/* ========================================================================== */

// STM32 HAL 头文件
#include "stm32f1xx_hal.h"  // 根据你的芯片型号修改
#include "cmsis_os.h"       // FreeRTOS 头文件
#include "FreeRTOS.h"
#include "stdlib.h"
#include "usart.h"
// 项目日志头文件
#include "log.h"

#define emMCP_printf    log_printf
#define emMCP_malloc    pvPortMalloc
#define emMCP_free      vPortFree
#define emMCP_delay     osDelay
#define emMCP_uart_send(data, len)  HAL_UART_Transmit(&huart2, (uint8_t*)(data), (len), HAL_MAX_DELAY)
#ifdef __cplusplus
}
#endif
#endif // __EMMCP_PORT_CONFIG_H__
