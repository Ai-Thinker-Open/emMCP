/**
 * @file emMCPLOG.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-30
 * 
 * @copyright Ai-Thinker co.,ltd (c) 2025
 * 
 */

#ifndef _EM_MCP_LOG_H_
#define _EM_MCP_LOG_H_
#include <uartPort.h>

#define ANSI_COLOR_RESET "\033[0m"
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_RED "\033[31m"
#define ANSI_COLOR_YELLOW "\033[33m"
#define ANSI_COLOR_BLUE "\033[34m"
#define ANSI_COLOR_MAGENTA "\033[35m"
#define ANSI_COLOR_CYAN "\033[36m"

// 日志级别枚举
typedef enum {
  emMCP_LOG_LEVEL_DEBUG,
  emMCP_LOG_LEVEL_INFO,
  emMCP_LOG_LEVEL_WARN,
  emMCP_LOG_LEVEL_ERROR,
  emMCP_LOG_LEVEL_FATAL
} emMCP_LogLevel;

extern emMCP_LogLevel log_level;
// 日志输出宏定义
#define emMCP_log_debug(...)                                                         \
  do {                                                                         \
    if (log_level <=  emMCP_LOG_LEVEL_DEBUG) {                                      \
      emMCP_PRINTF(ANSI_COLOR_BLUE "[DEBUG] %s:%d: ", __func__, __LINE__);       \
      emMCP_PRINTF(__VA_ARGS__);                                                 \
      emMCP_PRINTF(ANSI_COLOR_RESET "\r\n");                                     \
    }                                                                          \
  } while (0)

#define emMCP_log_info(...)                                                          \
  do {                                                                         \
    if (log_level <=  emMCP_LOG_LEVEL_INFO) {                                       \
      emMCP_PRINTF(ANSI_COLOR_GREEN "[INFO] %s:%d: ", __func__, __LINE__);       \
      emMCP_PRINTF(__VA_ARGS__);                                                 \
      emMCP_PRINTF(ANSI_COLOR_RESET "\r\n");                                     \
    }                                                                          \
  } while (0)

#define emMCP_log_warn(...)                                                          \
  do {                                                                         \
    if (log_level <=  emMCP_LOG_LEVEL_WARN) {                                       \
      emMCP_PRINTF(ANSI_COLOR_YELLOW "[WARN] %s:%d: ", __func__, __LINE__);      \
      emMCP_PRINTF(__VA_ARGS__);                                                 \
      emMCP_PRINTF(ANSI_COLOR_RESET "\r\n");                                     \
    }                                                                          \
  } while (0)

#define emMCP_log_error(...)                                                         \
  do {                                                                         \
    if (log_level <=  emMCP_LOG_LEVEL_ERROR) {                                      \
      emMCP_PRINTF(ANSI_COLOR_RED "[ERROR] %s:%d: ", __func__, __LINE__);        \
      emMCP_PRINTF(__VA_ARGS__);                                                 \
      emMCP_PRINTF(ANSI_COLOR_RESET "\r\n");                                     \
    }                                                                          \
  } while (0)

#define emMCP_log_fatal(...)                                                         \
  do {                                                                         \
    if (log_level <=  emMCP_LOG_LEVEL_FATAL) {                                      \
      emMCP_PRINTF(ANSI_COLOR_MAGENTA "[FATAL] %s:%d: ", __func__, __LINE__);    \
      emMCP_PRINTF(__VA_ARGS__);                                                 \
      emMCP_PRINTF(ANSI_COLOR_RESET "\r\n");                                     \
    }                                                                          \
  } while (0)

#endif // _EM_MCP_LOG_H_