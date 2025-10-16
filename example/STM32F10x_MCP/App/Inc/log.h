/**
 * @file log.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-29
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef _LOG_H
#define _LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// ANSI颜色控制码
#define ANSI_COLOR_RESET "\033[0m"
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_RED "\033[31m"
#define ANSI_COLOR_YELLOW "\033[33m"
#define ANSI_COLOR_BLUE "\033[34m"
#define ANSI_COLOR_MAGENTA "\033[35m"
#define ANSI_COLOR_CYAN "\033[36m"

// 日志级别枚举
typedef enum
{
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL
} LogLevel;

void log_init(void);
void log_printf(const char *format, ...);
extern LogLevel g_log_level;
// 日志输出宏定义
#define log_debug(...)                                                   \
  do                                                                     \
  {                                                                      \
    if (g_log_level <= LOG_LEVEL_DEBUG)                                  \
    {                                                                    \
      log_printf(ANSI_COLOR_BLUE "[DEBUG] %s:%d: ", __func__, __LINE__); \
      log_printf(__VA_ARGS__);                                           \
      log_printf(ANSI_COLOR_RESET "\r\n");                               \
    }                                                                    \
  } while (0)

#define log_info(...)                                                    \
  do                                                                     \
  {                                                                      \
    if (g_log_level <= LOG_LEVEL_INFO)                                   \
    {                                                                    \
      log_printf(ANSI_COLOR_GREEN "[INFO] %s:%d: ", __func__, __LINE__); \
      log_printf(__VA_ARGS__);                                           \
      log_printf(ANSI_COLOR_RESET "\r\n");                               \
    }                                                                    \
  } while (0)

#define log_warn(...)                                                     \
  do                                                                      \
  {                                                                       \
    if (g_log_level <= LOG_LEVEL_WARN)                                    \
    {                                                                     \
      log_printf(ANSI_COLOR_YELLOW "[WARN] %s:%d: ", __func__, __LINE__); \
      log_printf(__VA_ARGS__);                                            \
      log_printf(ANSI_COLOR_RESET "\r\n");                                \
    }                                                                     \
  } while (0)

#define log_error(...)                                                  \
  do                                                                    \
  {                                                                     \
    if (g_log_level <= LOG_LEVEL_ERROR)                                 \
    {                                                                   \
      log_printf(ANSI_COLOR_RED "[ERROR] %s:%d: ", __func__, __LINE__); \
      log_printf(__VA_ARGS__);                                          \
      log_printf(ANSI_COLOR_RESET "\r\n");                              \
    }                                                                   \
  } while (0)

#define log_fatal(...)                                                      \
  do                                                                        \
  {                                                                         \
    if (g_log_level <= LOG_LEVEL_FATAL)                                     \
    {                                                                       \
      log_printf(ANSI_COLOR_MAGENTA "[FATAL] %s:%d: ", __func__, __LINE__); \
      log_printf(__VA_ARGS__);                                              \
      log_printf(ANSI_COLOR_RESET "\r\n");                                  \
    }                                                                       \
  } while (0)

#endif