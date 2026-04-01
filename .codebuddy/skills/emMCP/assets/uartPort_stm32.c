/**
 * @file uartPort_stm32.c
 * @brief STM32 HAL UART implementation for emMCP
 * @version 1.0.0
 */

#include "uartPort.h"
#include "usart.h"  // HAL UART header

extern UART_HandleTypeDef huart2;  // 修改为你的UART句柄
static char uart_rx_buffer[256];

/**
 * @brief 串口发送数据
 */
int uartPortSendData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        &huart2, 
        (uint8_t *)data, 
        len, 
        1000
    );
    
    return (status == HAL_OK) ? 0 : -1;
}

/**
 * @brief 串口接收数据处理
 */
int uartPortRecvData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    
    // 释放旧的缓冲区
    if (uart_data_buf != NULL) {
        emMCP_free(uart_data_buf);
    }
    
    // 分配新缓冲区并复制数据
    uart_data_buf = (char *)emMCP_malloc(len + 1);
    if (uart_data_buf != NULL) {
        memcpy(uart_data_buf, data, len);
        uart_data_buf[len] = '\0';
        emMCP_UpdateUartRecv(true);
    }
    
    return 0;
}

/**
 * @brief UART中断回调函数 - 添加到stm32fxxx_it.c
 * 
 * void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
 * {
 *     if (huart->Instance == USART2) {
 *         uartPortRecvData((char *)uart_rx_buffer, Size);
 *         HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)uart_rx_buffer, sizeof(uart_rx_buffer));
 *         __HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
 *     }
 * }
 */
