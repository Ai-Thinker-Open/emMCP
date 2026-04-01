# emMCP Porting Guide

This guide explains how to port emMCP to different MCU platforms.

## Required Files

Copy these folders to your project:
- `uart-mcp/` - Core library (do not modify)
- `port/` - Platform-specific implementation (modify as needed)

## Platform-Specific Configuration

### STM32 HAL (with FreeRTOS)

#### 1. Create emMCP_port_config.h

```c
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"

// Print function
#define emMCP_printf printf

// Memory management
#define emMCP_malloc pvPortMalloc
#define emMCP_free vPortFree

// Delay function
#define emMCP_delay osDelay

#endif
```

#### 2. Implement uartPort.c

```c
#include "uartPort.h"
#include "usart.h"  // Your HAL UART header

extern UART_HandleTypeDef huart2;  // Your UART handle
static char uart_rx_buffer[256];

int uartPortSendData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 1000);
    return (status == HAL_OK) ? 0 : -1;
}

int uartPortRecvData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    // Copy to internal buffer
    if (uart_data_buf != NULL) {
        emMCP_free(uart_data_buf);
    }
    uart_data_buf = emMCP_malloc(len + 1);
    if (uart_data_buf != NULL) {
        memcpy(uart_data_buf, data, len);
        uart_data_buf[len] = '\0';
        emMCP_UpdateUartRecv(true);
    }
    return 0;
}
```

#### 3. Configure UART Reception (DMA + Idle Line Detection)

In `main.c` or initialization:

```c
// Start DMA reception with idle line detection
HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)uart_rx_buffer, sizeof(uart_rx_buffer));
__HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
```

UART callback:

```c
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2) {
        uartPortRecvData((char *)uart_rx_buffer, Size);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)uart_rx_buffer, sizeof(uart_rx_buffer));
        __HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
    }
}
```

---

### ESP32 (ESP-IDF)

#### 1. Create emMCP_port_config.h

```c
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Print function
#define emMCP_printf printf

// Memory management
#define emMCP_malloc malloc
#define emMCP_free free

// Delay function
#define emMCP_delay(ms) vTaskDelay(pdMS_TO_TICKS(ms))

#endif
```

#### 2. Implement uartPort.c

```c
#include "uartPort.h"
#include "driver/uart.h"

#define EMCP_UART_NUM UART_NUM_2
#define EMCP_UART_BUF_SIZE 256

static char uart_rx_buffer[EMCP_UART_BUF_SIZE];

int uartPortSendData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    int ret = uart_write_bytes(EMCP_UART_NUM, data, len);
    return (ret == len) ? 0 : -1;
}

int uartPortRecvData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    if (uart_data_buf != NULL) {
        emMCP_free(uart_data_buf);
    }
    uart_data_buf = emMCP_malloc(len + 1);
    if (uart_data_buf != NULL) {
        memcpy(uart_data_buf, data, len);
        uart_data_buf[len] = '\0';
        emMCP_UpdateUartRecv(true);
    }
    return 0;
}
```

#### 3. UART Reception Task

```c
void emMCP_uart_rx_task(void *pvParameters)
{
    uint8_t data[EMCP_UART_BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(EMCP_UART_NUM, data, EMCP_UART_BUF_SIZE, pdMS_TO_TICKS(10));
        if (len > 0) {
            uartPortRecvData((char *)data, len);
        }
    }
}
```

---

### Arduino

#### 1. Create emMCP_port_config.h

```c
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include <Arduino.h>

// Print function
#define emMCP_printf Serial.printf

// Memory management
#define emMCP_malloc malloc
#define emMCP_free free

// Delay function
#define emMCP_delay delay

#endif
```

#### 2. Implement uartPort.c (Arduino sketch)

```cpp
#include "uartPort.h"

HardwareSerial &emMCP_Serial = Serial2;  // Use Serial2

int uartPortSendData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    emMCP_Serial.write((uint8_t *)data, len);
    return 0;
}

int uartPortRecvData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    if (uart_data_buf != NULL) {
        emMCP_free(uart_data_buf);
    }
    uart_data_buf = (char *)emMCP_malloc(len + 1);
    if (uart_data_buf != NULL) {
        memcpy(uart_data_buf, data, len);
        uart_data_buf[len] = '\0';
        emMCP_UpdateUartRecv(true);
    }
    return 0;
}

// Call this in loop()
void emMCP_check_uart()
{
    while (emMCP_Serial.available()) {
        static char buffer[256];
        static int index = 0;
        
        char c = emMCP_Serial.read();
        buffer[index++] = c;
        
        if (c == '\n' || index >= sizeof(buffer) - 1) {
            buffer[index] = '\0';
            uartPortRecvData(buffer, index);
            index = 0;
        }
    }
}
```

---

### Generic Platform (No OS)

#### 1. Create emMCP_port_config.h

```c
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>

// Platform detection - define your platform
#define EMCP_PLATFORM_GENERIC

// Print function
#define emMCP_printf printf

// Memory management
#define emMCP_malloc malloc
#define emMCP_free free

// Delay function - implement this
void my_delay_ms(uint32_t ms);  // Declare your delay function
#define emMCP_delay my_delay_ms

#endif
```

#### 2. Implement Delay Function

```c
// For SysTick-based delay
void my_delay_ms(uint32_t ms)
{
    uint32_t start = get_systick_counter();
    while ((get_systick_counter() - start) < ms);
}
```

---

## CMake Integration

### Option 1: Add as Subdirectory

```cmake
add_subdirectory(uart-mcp)
target_link_libraries(your_target PRIVATE emMCP)
```

### Option 2: Direct Source Inclusion

```cmake
set(EMCP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp/emMCP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp/cJSON/cJSON.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/uartPort.c
)

target_sources(your_target PRIVATE ${EMCP_SOURCES})
target_include_directories(your_target PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp
    ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp/cJSON
    ${CMAKE_CURRENT_SOURCE_DIR}/port
)
```

---

## Build Flags

Add these to your compiler flags:

```cmake
# For cJSON
target_compile_definitions(your_target PRIVATE 
    CJSON_USE_CUSTOM_ALLOCATOR  # Optional: use custom allocator
)

# For emMCP configuration
target_compile_definitions(your_target PRIVATE 
    MCP_SERVER_TOOL_NUMBLE_MAX=5  # Increase max tools if needed
)
```

---

## Testing Your Port

### 1. Basic Initialization Test

```c
void test_init(void)
{
    emMCP_t dev;
    int ret = emMCP_Init(&dev);
    assert(ret == 0);
    printf("emMCP initialized successfully\n");
}
```

### 2. UART Loopback Test

```c
void test_uart(void)
{
    char test_msg[] = "test message";
    uartPortSendData(test_msg, strlen(test_msg));
    // Check if echoed back
}
```

### 3. Event Callback Test

```c
volatile int event_received = 0;

void emMCP_EventCallback(emMCP_event_t event, mcp_server_tool_type_t type, void *param)
{
    event_received = 1;
    printf("Event: %d, Param: %s\n", event, (char *)param);
}

void test_events(void)
{
    // Send wake command or wait for device to send status
    emMCP_TickHandle(10);
    assert(event_received);
}
```

---

## Troubleshooting

### Issue: Compilation Error - emMCP_delay not defined

**Solution:** Ensure `emMCP_port_config.h` is in your include path and all macros are defined.

### Issue: Linker Error - undefined reference to uartPortSendData

**Solution:** Make sure `uartPort.c` is being compiled and linked.

### Issue: No UART data received

**Solution:** 
- Check UART initialization (baudrate, pins)
- Verify interrupt/DMA is enabled
- Confirm `uartPortRecvData()` is called from ISR

### Issue: Events not triggering

**Solution:**
- Verify `emMCP_TickHandle()` is called in main loop
- Check that `uartPortRecvData()` sets the receive flag
