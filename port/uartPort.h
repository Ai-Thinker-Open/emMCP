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

#include "stdbool.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 用户自定义配置文件
 * 
 * emMCP 支持多种方式来指定用户配置文件：
 *
 * 方式1：直接指定完整文件路径（推荐用于自定义文件名）
 *   在 CMake 中: target_compile_definitions(your_target PRIVATE 
 *       EMCP_USER_CONFIG_FILE=\"path/to/your_config.h\"
 *   )
 *   或在代码中: #define EMCP_USER_CONFIG_FILE "path/to/your_config.h"
 *
 * 方式2：使用 __has_include 自动检测（推荐用于标准配置）
 *   只要 emMCP_port_config.h 在包含路径中，就会自动包含
 *   支持自定义文件名：在包含 emMCP 之前定义 EMCP_CONFIG_FILENAME
 *
 * 方式3：直接定义所有宏（最简单，无配置文件）
 *   在包含 uartPort.h 之前直接定义：
 *   #define emMCP_printf log_printf
 *   #define emMCP_malloc pvPortMalloc
 *   ...等等
 */

// 方式1：直接指定完整文件路径
#ifdef EMCP_USER_CONFIG_FILE
    #include EMCP_USER_CONFIG_FILE
    #define __EMMCP_CONFIG_INCLUDED__
#endif

// 方式2：自动检测配置文件（如果方式1未使用）
#ifndef __EMMCP_CONFIG_INCLUDED__
    // 允许用户自定义配置文件名（默认为 emMCP_port_config.h）
    #ifndef EMCP_CONFIG_FILENAME
        #define EMCP_CONFIG_FILENAME "emMCP_port_config.h"
    #endif
    
    // 使用 __has_include 检测文件是否存在（C++17 或 GCC 扩展）
    #if defined(__has_include)
        #if __has_include(EMCP_CONFIG_FILENAME)
            #include EMCP_CONFIG_FILENAME
            #define __EMMCP_CONFIG_INCLUDED__
        #endif
    #else
        // 不支持 __has_include 时，直接尝试包含
        #include EMCP_CONFIG_FILENAME
        #define __EMMCP_CONFIG_INCLUDED__
    #endif
#endif

#include "emMCP.h"

/**
 * @brief 默认实现声明
 * 
 * 如果用户没有定义相应的宏，则使用这些默认实现
 */
#ifndef emMCP_printf
    // 默认使用标准库的 printf，但只在有标准输出时启用
    #ifdef __STDC_HOSTED__
        #include <stdio.h>
        #define emMCP_printf printf
    #else
        // 嵌入式环境，默认使用空实现
        #define emMCP_printf(...) ((void)0)
    #endif
#endif

#ifndef emMCP_malloc
    #ifdef __STDC_HOSTED__
        #include <stdlib.h>
        #define emMCP_malloc malloc
    #else
        // 嵌入式环境，需要用户提供实现
        #error "emMCP_malloc must be defined in embedded environments. Please define emMCP_malloc in your emMCP_port_config.h or before including uartPort.h"
    #endif
#endif

#ifndef emMCP_free
    #ifdef __STDC_HOSTED__
        #include <stdlib.h>
        #define emMCP_free free
    #else
        // 嵌入式环境，需要用户提供实现
        #error "emMCP_free must be defined in embedded environments. Please define emMCP_free in your emMCP_port_config.h or before including uartPort.h"
    #endif
#endif

#ifndef emMCP_delay
    // 延时函数必须由用户提供，因为不同平台的延时实现差异很大
    #error "emMCP_delay must be defined. Please define emMCP_delay in your emMCP_port_config.h or before including uartPort.h. Example: #define emMCP_delay HAL_Delay"
#endif

#ifndef emMCP_uart_send
    #error "emMCP_uart_send is not defined"
#endif
/**
 * @brief 宏定义验证
 * 
 * 确保所有必需的宏都已正确定义
 */
#ifndef emMCP_printf
    #error "emMCP_printf is not defined"
#endif
#ifndef emMCP_malloc
    #error "emMCP_malloc is not defined"
#endif
#ifndef emMCP_free
    #error "emMCP_free is not defined"
#endif
#ifndef emMCP_delay
    #error "emMCP_delay is not defined"
#endif

/**
 * @brief 串口发送数据
 *
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return int 成功返回0，失败返回负值
 */
int uartPortSendData(char *data, int len);

/**
 * @brief 串口接收数据
 *
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return int 成功返回0，失败返回负值
 */
int uartPortRecvData(char *data, int len);

/**
 * @brief 设置UART数据缓冲区指针（内部使用）
 *
 * @param buf 缓冲区指针
 */
void uartPortSetDataBuf(char *buf);

/**
 * @brief 获取UART数据缓冲区指针（内部使用）
 *
 * @return char* 缓冲区指针
 */
char *uartPortGetDataBuf(void);

/**
 * @brief 获取接收到的数据（在任务上下文中调用）
 * 
 * @note 此函数从双缓冲区中获取数据，是线程安全的
 * @return char* 数据指针，NULL表示没有新数据
 */
char *uartPortGetRxData(void);

/**
 * @brief 标记数据已处理（在任务上下文中调用）
 * 
 * @note 调用此函数后，中断可以接收新数据
 */
void uartPortClearRxData(void);

/* 调试变量 - 可以在调试器中查看 */
extern volatile uint32_t g_uart_rx_count;
extern volatile uint32_t g_uart_rx_size;

#ifdef __cplusplus
}
#endif

#endif // __UART_PORT_H__
