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
#include "log.h"

#define emMCP_PRINTF log_printf

int uartPortSendData(char *data, int len);
int uartPortRecvData(char ch);
#endif // __UART_PORT_H__