/**
 * @file log.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-29
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "log.h"
#include "usart.h"

#define LOG_BUFFER_SIZE 512

LogLevel g_log_level = LOG_LEVEL_DEBUG;

/**
 * @brief
 *
 */
void log_printf(const char *format, ...)
{
  char buffer[LOG_BUFFER_SIZE]; // 用于存储格式化后的字符串
  va_list args;

  // 1. 初始化可变参数列表
  va_start(args, format);
  int len = vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);
  va_end(args);
  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len, 100);
  //   HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buffer, len);
}
// 启动下一条队列数据的 DMA 发送

/**
 * @brief 打印错误日志
 *
 */
void log_init(void)
{
  // 初始化串口1

  MX_USART1_UART_Init();
}
