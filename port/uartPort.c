/**
 * @file uartPort.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include "uartPort.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
#include "usart.h"

/**
 * @brief 串口发送函数接口
 *
 * @param data
 * @param len
 * @return int
 */
bool uartPortSendData(char *data, int len)
{
    // 在此处实现串口发送函数
    if (data == NULL || len <= 0)
    {
        return false;
    }
    // TODO: 实现串口发送函数
    // 例如：HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 1000);
    HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 1000);
    return true;
}
/**
 * @brief 串口接收函数接口,把这个函数在串口接收中断或接收循环中调用
 *
 * @param data
 * @param len
 * @return int
 */
bool uartPortRecvData(char *data, int len)
{
    if (data == NULL)
    {
        emMCP_log_error("uartPortRecvData: data is NULL");
        return false;
    }
    // emMCP_log_info("%s", data);
    memset(uart_data_buf, 0, sizeof(uart_data_buf));
    memcpy(uart_data_buf, data, len);
    // UART_RECV = 1;
    return true;
}
