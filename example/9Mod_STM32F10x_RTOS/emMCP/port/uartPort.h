/**
 * @file uartPort.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */

#ifndef __UART_PORT_H__
#define __UART_PORT_H__

#include "emMCP.h"
#include "stdbool.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "log.h"
/**
 * @brief 定义串口打印函数
 *
 */
#define emMCP_printf log_printf
/**
 * @brief 定义内存操作函数
 *
 */
#define emMCP_malloc pvPortMalloc
#define emMCP_free vPortFree

#ifndef emMCP_malloc
#error "emMCP_malloc is not defined"
#endif
#ifndef emMCP_free
#error "emMCP_free is not defined"
#endif
/**
 * @brief 定义延时函数
 *
 */
#define emMCP_delay osDelay

#ifndef emMCP_delay
#error "emMCP_delay is not defined"
#endif
/**
 * @brief 初始化串口
 *
 * @param data
 * @param len
 * @return int
 */
int uartPortSendData(char *data, int len);
/**
 * @brief 接收串口数据
 *
 * @param ch
 * @return int
 */
int uartPortRecvData(char *data, int len);

#endif // __UART_PORT_H__