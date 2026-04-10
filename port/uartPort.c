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

/* 双缓冲机制 - 避免在中断中进行内存分配 */
#define UART_RX_BUF_SIZE 512
static char uart_rx_buffer[2][UART_RX_BUF_SIZE];
static volatile uint8_t rx_buf_index = 0;
static volatile uint8_t rx_data_ready = 0;
static volatile uint16_t rx_data_len = 0;

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
 * @note 此函数在中断上下文中调用，不能调用FreeRTOS的内存管理函数！
 *
 * @param data 数据缓冲区
 * @param len 缓冲区长度
 * @return int 成功返回0，失败返回负值
 */
int uartPortRecvData(char *data, int len)
{
    if (data == NULL || len <= 0 || len >= UART_RX_BUF_SIZE)
    {
        return -1;
    }

    // 使用当前缓冲区索引
    uint8_t current_idx = rx_buf_index;
    
    // 复制数据到静态缓冲区（在中断中是安全的）
    memcpy(uart_rx_buffer[current_idx], data, len);
    uart_rx_buffer[current_idx][len] = '\0';
    
    // 更新状态
    rx_data_len = len;
    rx_data_ready = 1;
    
    // 切换到另一个缓冲区（为下一次接收做准备）
    rx_buf_index = (current_idx + 1) % 2;

    // 调试计数
    g_uart_rx_count++;
    g_uart_rx_size = len;

    emMCP_UpdateUartRecv(true);
    return 0;
}

// 调试用的变量，可以在调试器中查看
volatile uint32_t g_uart_rx_count = 0;
volatile uint32_t g_uart_rx_size = 0;

/**
 * @brief 获取接收到的数据（在任务上下文中调用）
 * 
 * @return char* 数据指针，NULL表示没有新数据
 */
char *uartPortGetRxData(void)
{
    if (!rx_data_ready)
    {
        return NULL;
    }
    
    // 返回当前使用的缓冲区（非rx_buf_index指向的那个）
    uint8_t data_idx = (rx_buf_index + 1) % 2;
    return uart_rx_buffer[data_idx];
}

/**
 * @brief 标记数据已处理（在任务上下文中调用）
 */
void uartPortClearRxData(void)
{
    rx_data_ready = 0;
    rx_data_len = 0;
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
