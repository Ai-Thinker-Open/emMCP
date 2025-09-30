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

#define LOG_BUFFER_SIZE 256
LogLevel g_log_level = LOG_LEVEL_DEBUG;
/**
 * @brief 打印日志
 *
 * @param format
 * @param ...
 */
void log_printf(const char *format, ...) {
  char buffer[LOG_BUFFER_SIZE]; // 用于存储格式化后的字符串
  va_list args;

  // 1. 初始化可变参数列表
  va_start(args, format);

  // 2. 将格式化字符串（含可变参数）写入缓冲区
  // vsnprintf：将格式化结果写入buffer，最多写LOG_BUFFER_SIZE-1个字符（留1个给'\0'）
  vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);

  // 3. 结束可变参数处理
  va_end(args);

  // 4. 通过串口发送缓冲区中的格式化字符串（不含末尾的'\0'）
  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 1000);
}

/**
 * @brief 打印错误日志
 *
 */
void log_init(void) {
  // 初始化串口1

  MX_USART1_UART_Init();
}