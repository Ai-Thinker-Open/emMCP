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
/**
 * @brief LOG 接口
 *
 */
#include "log.h"

/**
 * @brief  系统内存头文件
 *
 */
#include "FreeRTOS.h"
/**
 * @brief 定义串口打印函数
 *
 */

#define emMCP_PRINTF log_printf

#define emMCP_malloc pvPortMalloc
#define emMCP_free vPortFree
/**
 * @brief 初始化串口
 *
 * @param data
 * @param len
 * @return int
 */
bool uartPortSendData(char *data, int len);
/**
 * @brief 接收串口数据
 *
 * @param ch
 * @return int
 */
bool uartPortRecvData(char *data, int len);

#endif // __UART_PORT_H__