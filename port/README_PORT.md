# emMCP Port 配置指南

本文档介绍如何使用新的 `port/uartPort.h` 配置系统来避免不同工程间的宏定义冲突。

## 目录

- [问题背景](#问题背景)
- [解决方案概述](#解决方案概述)
- [使用方法](#使用方法)
- [配置示例](#配置示例)
- [CMake 集成](#cmake-集成)
- [常见问题](#常见问题)

## 问题背景

### 原始痛点

在旧的实现中，`port/uartPort.h` 包含以下代码：

```c
#define emMCP_printf log_printf
#define emMCP_malloc pvPortMalloc
#define emMCP_free vPortFree
#define emMCP_delay osDelay
```

这导致以下问题：

1. **命名冲突**：不同工程可能使用不同的函数名（如 `log_printf` vs `printf` vs `SEGGER_RTT_printf`）
2. **内存管理器差异**：裸机项目使用 `malloc/free`，FreeRTOS 使用 `pvPortMalloc/vPortFree`，RT-Thread 使用 `rt_malloc/rt_free`
3. **延时函数差异**：STM32 HAL 使用 `HAL_Delay`，FreeRTOS 使用 `osDelay`，裸机可能需要自定义实现

3. **维护困难**：修改 `port/uartPort.h` 会影响所有使用该库的工程

## 解决方案概述

新的 `port/uartPort.h` 采用**条件编译 + 用户配置**的设计：

### 核心机制

1. **默认回退**：如果用户没有定义宏，则使用合理的默认值（如标准库的 `printf`/`malloc`）
2. **错误提示**：对于嵌入式环境必须的函数（如 `emMCP_delay`），提供清晰的错误信息
3. **配置注入**：支持通过 CMake 或预处理器定义注入用户配置

### 宏定义优先级

```
用户定义 > 平台自动检测 > 标准库默认值 > 编译错误
```

## 使用方法

### 方法 1：直接定义宏（简单项目）

在使用 `uartPort.h` 之前直接定义宏：

```c
// 在你的 main.c 或配置头文件中
#define emMCP_printf    log_printf
#define emMCP_malloc    pvPortMalloc
#define emMCP_free      vPortFree
#define emMCP_delay     osDelay

// 然后包含 emMCP 头文件
#include "uartPort.h"
```

### 方法 2：创建配置文件（推荐）

1. 在你的项目目录中创建 `emMCP_port_config.h`：

```c
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

// 平台头文件
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

// 日志函数
#include "log.h"
#define emMCP_printf    log_printf

// 内存管理（使用 FreeRTOS）
#define emMCP_malloc    pvPortMalloc
#define emMCP_free      vPortFree

// 延时函数
#define emMCP_delay     osDelay

#endif
```

2. 使用以下任一方式包含配置文件：

**选项 A**：通过 CMake 添加包含路径
```cmake
# 在父项目的 CMakeLists.txt 中
add_subdirectory(path/to/emMCP)
target_include_directories(your_target BEFORE PRIVATE path/to/your/config)
```

**选项 B**：通过编译定义指定配置文件
```cmake
target_compile_definitions(your_target PRIVATE 
    EMCP_USER_CONFIG_FILE=\"path/to/your/emMCP_port_config.h\"
)
```

### 方法 3：使用 CMake 辅助函数（高级）

emMCP 提供了 CMake 辅助函数简化配置：

```cmake
# 添加 emMCP 子目录
add_subdirectory(path/to/emMCP)

# 方法 1：设置用户配置路径
emcp_set_user_config(your_target "${CMAKE_CURRENT_SOURCE_DIR}/config")

# 方法 2：添加宏定义
emcp_add_definitions(your_target 
    EMCP_USER_CONFIG_FILE=\"emMCP_port_config.h\"
)

# 链接 emMCP 库
target_link_libraries(your_target emMCP)
```

## 配置示例

### 示例 1：STM32 HAL + FreeRTOS（最常见）

```c
// emMCP_port_config.h
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "log.h"

#define emMCP_printf    log_printf
#define emMCP_malloc    pvPortMalloc
#define emMCP_free      vPortFree
#define emMCP_delay     osDelay

#endif
```

### 示例 2：裸机 STM32（无操作系统）

```c
// emMCP_port_config.h
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>

// 使用标准库 printf
#define emMCP_printf    printf

// 使用标准库内存管理
#define emMCP_malloc    malloc
#define emMCP_free      free

// 使用 HAL 延时
#define emMCP_delay     HAL_Delay

#endif
```

### 示例 3：Linux/Unix 主机环境

```c
// emMCP_port_config.h
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 使用标准库函数
#define emMCP_printf    printf
#define emMCP_malloc    malloc
#define emMCP_free      free

// 自定义延时函数（微秒转毫秒）
static inline void emcp_delay_ms(uint32_t ms) {
    usleep(ms * 1000);
}
#define emMCP_delay(ms) emcp_delay_ms(ms)

#endif
```

## CMake 集成

### 基础用法

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyProject)

# 添加 emMCP
add_subdirectory(path/to/emMCP)

# 创建可执行文件
add_executable(my_app main.c)

# 链接 emMCP
target_link_libraries(my_app emMCP)
```

### 使用用户配置

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyProject)

# 方法 1：通过 CMake 变量设置配置路径
set(EMCP_USER_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config" CACHE PATH "emMCP config path")

# 添加 emMCP
add_subdirectory(path/to/emMCP)

# 创建可执行文件
add_executable(my_app main.c)

# 链接 emMCP
target_link_libraries(my_app emMCP)
```

### 使用辅助函数

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyProject)

# 添加 emMCP
add_subdirectory(path/to/emMCP)

# 创建可执行文件
add_executable(my_app main.c)

# 使用 emMCP 辅助函数设置配置
emcp_set_user_config(my_app "${CMAKE_CURRENT_SOURCE_DIR}/config")

# 或者使用宏定义方式
# emcp_add_definitions(my_app 
#     EMCP_USER_CONFIG_FILE=\"emMCP_port_config.h\"
# )

# 链接 emMCP
target_link_libraries(my_app emMCP)
```

## 常见问题

### Q1: 编译时提示 "emMCP_delay must be defined"

**原因**：没有定义 `emMCP_delay` 宏。

**解决方法**：
1. 创建 `emMCP_port_config.h` 文件
2. 定义延时函数：
```c
#define emMCP_delay HAL_Delay  // 或 osDelay，或自定义函数
```
3. 将配置路径添加到编译器的包含路径中

### Q2: 不同项目使用不同的内存管理器

**解决方法**：
在每个项目的配置文件中定义适合自己平台的内存管理函数：

```c
// FreeRTOS 项目
#define emMCP_malloc pvPortMalloc
#define emMCP_free vPortFree

// 裸机项目
#define emMCP_malloc malloc
#define emMCP_free free

// RT-Thread 项目
#define emMCP_malloc rt_malloc
#define emMCP_free rt_free
```

### Q3: 如何在同一个项目中支持多个平台

**解决方法**：
使用条件编译：

```c
// emMCP_port_config.h
#ifdef USE_FREERTOS
    #define emMCP_malloc pvPortMalloc
    #define emMCP_free vPortFree
    #define emMCP_delay osDelay
#else
    #define emMCP_malloc malloc
    #define emMCP_free free
    #define emMCP_delay HAL_Delay
#endif
```

然后在 CMake 中：
```cmake
if(USE_FREERTOS)
    target_compile_definitions(my_app PRIVATE USE_FREERTOS)
endif()
```

### Q4: 编译器提示头文件找不到

**原因**：配置文件路径没有正确添加到包含路径中。

**解决方法**：
确保在 CMake 中正确设置了配置路径：

```cmake
# 方法 1：使用全局变量
set(EMCP_USER_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
add_subdirectory(emMCP)

# 方法 2：使用辅助函数
add_subdirectory(emMCP)
emcp_set_user_config(your_target "${CMAKE_CURRENT_SOURCE_DIR}/config")

# 方法 3：手动添加包含路径
target_include_directories(your_target BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/config")
```

### Q5: 如何调试宏定义问题

**解决方法**：
1. 检查编译器输出，查看具体的错误信息
2. 使用预处理命令查看宏展开结果：
```bash
# GCC
arm-none-eabi-gcc -E -dM your_file.c | grep emMCP_
```
3. 在配置文件中添加调试输出：
```c
#pragma message "emMCP_malloc is defined as: " XSTR(emMCP_malloc)
```

## 总结

新的 `port/uartPort.h` 设计解决了以下问题：

1. **避免宏定义冲突**：每个项目可以使用自己的函数名，不再强制使用特定的命名
2. **跨平台支持**：通过配置文件自动适配不同平台（STM32 HAL、FreeRTOS、RT-Thread 等）
3. **灵活的配置方式**：支持多种配置方式（直接定义、配置文件、CMake 注入）
4. **更好的错误提示**：当配置不正确时，提供清晰的错误信息
5. **向后兼容**：现有项目可以通过简单的修改迁移到新系统
