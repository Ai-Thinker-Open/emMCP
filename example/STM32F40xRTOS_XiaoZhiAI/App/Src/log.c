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
#include "FreeRTOS.h"
#include "main.h"
#include "portable.h"
#include "stdio.h"
#include "usart.h"
#include <string.h>

#define LOG_BUFFER_SIZE 2048

LogLevel g_log_level = LOG_LEVEL_DEBUG;
char buffer[LOG_BUFFER_SIZE];
/**
 * @brief
 *
 */
void log_printf(const char *format, ...) {

  va_list args;
  memset(buffer, 0, 1024);
  // 1. 初始化可变参数列表
  va_start(args, format);
  int len = vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);
  va_end(args);
  if (len <= 0 || len >= LOG_BUFFER_SIZE) {
    // 处理异常：长度无效或字符串被截断
    return;
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len, 100);
}
/**
 * @brief 打印错误日志
 *
 */
void log_init(void) {
  // 初始化串口1

  MX_USART1_UART_Init();
}
