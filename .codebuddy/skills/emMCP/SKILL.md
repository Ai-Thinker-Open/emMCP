---
name: emMCP
description: This skill provides comprehensive guidance for developing, debugging, and maintaining emMCP firmware. It covers the AiPi-PalChatV1-based AI voice chat device SDK, including build system, flash procedures, UART-MCP integration, and common bug fixes. This skill should be used when working with emMCP, AiPi-PalChatV1, or any UART-MCP-based voice AI MCU projects.
---

# emMCP Skill

## Overview

emMCP (Easy MCU MCP) is an adapter library for rapid development based on Ai-Thinker UART-MCP protocol. It provides a simple API set to help developers quickly use MCU to access AI models for MCP interaction.

**Key Features:**
- Support for customizing MCP tools to AI agents
- Support for actively waking up Ai-Thinker AI devices
- Support for setting communication baud rate (default 115200)
- Support for setting conversation volume
- Built-in simple state machine with customizable callback functions
- Built-in MCP tool management
- Minimal resource usage: RAM 62 Byte, FLASH 1708 Byte (minimum)

## When to Use This Skill

Use this skill when:
- Working with emMCP source code (emMCP.c, emMCP.h)
- Porting emMCP to new MCU platforms (STM32, ESP32, etc.)
- Creating MCP tools for AI interaction
- Debugging UART-MCP communication issues
- Configuring emMCP port settings
- Developing voice AI applications with Ai-Thinker modules

## Project Structure

```
emMCP/
├── uart-mcp/           # Core emMCP library
│   ├── emMCP.c         # Main implementation
│   ├── emMCP.h         # Public API header
│   ├── emMCPLOG.h      # Logging macros
│   └── cJSON/          # JSON parser dependency
├── port/               # Platform porting layer
│   ├── uartPort.c      # UART implementation template
│   ├── uartPort.h      # UART header
│   ├── emMCP_port_config_template.h  # Configuration template
│   └── README_PORT.md  # Porting instructions
└── example/            # Example projects
    └── STM32F10xRTOS_MCP/  # STM32 FreeRTOS example
```

## Core Capabilities

### 1. Initialization and Main Loop

Initialize emMCP before use and call the tick handler in the main loop:

```c
static emMCP_t emMCP_dev;

int main(void)
{
    // Initialize emMCP
    if (emMCP_Init(&emMCP_dev) != 0) {
        // Handle error
    }
    
    while(1)
    {
        // Process emMCP events (call periodically)
        emMCP_TickHandle(10);
    }
}
```

### 2. Event Callback Handling

Implement the weak callback function to handle events:

```c
__emMCPWeak void emMCP_EventCallback(emMCP_event_t event, 
                                      mcp_server_tool_type_t type, 
                                      void *param)
{
    switch (event) {
        case emMCP_EVENT_AI_WAKE:
            // Device woke up
            break;
        case emMCP_EVENT_AI_MCP_CMD:
            // MCP command received
            break;
        case emMCP_EVENT_CMD_OK:
            // Command executed successfully
            break;
        // Handle other events...
    }
}
```

### 3. MCP Tool Registration

Create and register custom MCP tools:

```c
// Define tool callbacks
void ledControlHandler(void *args) {
    cJSON *params = (cJSON *)args;
    cJSON *state = emMCP_GetParam(params, "state");
    // Handle LED control logic
    emMCP_ResponseValue(emMCP_CTRL_OK);
}

// Create tool structure
emMCP_tool_t led_tool = {
    .name = "led_control",
    .description = "Control LED on/off",
    .setRequestHandler = ledControlHandler,
    .checkRequestHandler = NULL,
    .inputSchema = {
        .properties = {
            {
                .name = "state",
                .description = "LED state: on or off",
                .type = MCP_SERVER_TOOL_TYPE_STRING
            }
        }
    }
};

// Add tool and register
emMCP_AddToolToToolList(&led_tool);
emMCP_RegistrationTools();
```

### 4. Device Control Functions

```c
// Wake up AI device
emMCP_SetAiWakeUp(30);  // Wake for 30 seconds

// Set volume (0-100)
emMCP_SetAiVolume(80);

// Get current volume
uint8_t volume = emMCP_CheckAiVolume();

// Set baudrate
emMCP_SetBaudrate(115200);
```

## Porting Guide

### Step 1: Copy Required Files

Copy `uart-mcp/` and `port/` folders to your project.

### Step 2: Configure Platform Settings

Create `emMCP_port_config.h` based on the template:

```c
#ifndef __EMMCP_PORT_CONFIG_H__
#define __EMMCP_PORT_CONFIG_H__

// STM32 HAL example
#define emMCP_printf printf
#define emMCP_malloc malloc
#define emMCP_free free
#define emMCP_delay HAL_Delay

#endif
```

### Step 3: Implement UART Functions

Edit `port/uartPort.c` to implement UART send:

```c
int uartPortSendData(char *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }
    return HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 100);
}
```

In your UART receive interrupt/DMA callback:

```c
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2) {
        // Call emMCP receive function
        uartPortRecvData((char *)rxBuffer, Size);
        
        // Restart DMA reception
        HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)rxBuffer, sizeof(rxBuffer));
    }
}
```

## Event Types Reference

| Event | Description |
|-------|-------------|
| `emMCP_EVENT_AI_START` | AI device started |
| `emMCP_EVENT_AI_WAKE` | Device woke up |
| `emMCP_EVENT_AI_SLEEP` | Device went to sleep |
| `emMCP_EVENT_AI_WIFI_CONNECTED` | WiFi connected |
| `emMCP_EVENT_AI_WIFI_GOT_IP` | Got IP address |
| `emMCP_EVENT_AI_NETERR` | Network error |
| `emMCP_EVENT_AI_MCP_CMD` | MCP command received |
| `emMCP_EVENT_CMD_OK` | Command executed OK |
| `emMCP_EVENT_CMD_ERROR` | Command error |

## Resources

### references/

- `api_reference.md` - Complete API documentation
- `porting_guide.md` - Detailed platform porting instructions

### assets/

Porting templates and example configurations for various platforms.

## Common Issues

### Issue: emMCP_Init returns -1
**Cause**: NULL pointer passed
**Solution**: Ensure emMCP_t structure is properly allocated

### Issue: No events received
**Cause**: UART reception not properly configured
**Solution**: Check `uartPortRecvData()` is called in UART ISR

### Issue: MCP tools not working
**Cause**: Tools not registered or callback not set
**Solution**: Call `emMCP_RegistrationTools()` after adding all tools

### Issue: Callback not firing
**Cause**: `emMCP_TickHandle()` not called
**Solution**: Ensure tick handler is called periodically in main loop
