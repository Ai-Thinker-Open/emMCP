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

/* Private variables ---------------------------------------------------------*/
/* 注意：uart_data_buf 在 emMCP.c 中定义，这里使用 extern 声明 */
extern char *uart_data_buf;

/* Private function prototypes -----------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/**
 * @brief 串口发送函数接口
 *
 * @param data 要发送的数据
 * @param len 数据长度
 * @return int 成功返回0，失败返回负值
 */
int uartPortSendData(char *data, int len)
{
    // 在此处实现串口发送函数
    if (data == NULL || len <= 0)
    {
        return -1;
    }

    // TODO: 用户需要在这里实现具体的串口发送逻辑
    // 例如：HAL_UART_Transmit(&huart1, (uint8_t*)data, len, HAL_MAX_DELAY);
    emMCP_uart_send(data, len);
    return 0; // 返回发送状态
}

/**
 * @brief 串口接收函数接口，把这个函数在串口接收中断或接收循环中调用
 *
 * @param data 数据缓冲区
 * @param len 缓冲区长度
 * @return int 成功返回0，失败返回负值
 */
int uartPortRecvData(char *data, int len)
{
    (void)len; // 防止未使用警告

    if (data == NULL)
    {
        emMCP_log_error("uartPortRecvData: data is NULL");
        return -1;
    }
    uart_data_buf = data;
    emMCP_UpdateUartRecv(true);
    return 0;
}

/**
 * @brief 设置UART数据缓冲区指针
 *
 * @param buf 缓冲区指针
 */
void uartPortSetDataBuf(char *buf)
{
    uart_data_buf = buf;
}

/**
 * @brief 获取UART数据缓冲区指针
 *
 * @return char* 缓冲区指针
 */
char *uartPortGetDataBuf(void)
{
    return uart_data_buf;
}
