[![中文](https://img.shields.io/badge/Language-简体中文-green.svg)](README_zh.md)
<h1 align="center">emMCP</h1>
<div align="center">

<a href="https://github.com/Ai-Thinker-Open/emMCP/stargazers"><img src="https://img.shields.io/github/stars/Ai-Thinker-Open/emMCP" alt="Stars Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/network/members"><img src="https://img.shields.io/github/forks/Ai-Thinker-Open/emMCP" alt="Forks Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/pulls"><img src="https://img.shields.io/github/issues-pr/Ai-Thinker-Open/emMCP" alt="Pull Requests Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/issues"><img src="https://img.shields.io/github/issues/Ai-Thinker-Open/emMCP" alt="Issues Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/graphs/contributors"><img alt="GitHub contributors" src="https://img.shields.io/github/contributors/Ai-Thinker-Open/emMCP?color=2b9348"></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/blob/master/LICENSE"><img src="https://img.shields.io/github/license/Ai-Thinker-Open/emMCP?color=2b9348" alt="License Badge"/></a>

<i>Like this project? Please consider giving it a Star ⭐️ to help improve the project!</i>

</div>

## Welcome to emMCP

emMCP (Easy mcu MCP) is an adapter library for rapid development based on [Ai-Thinker UART-MCP](https://docs.ai-thinker.com/uart_mcp/). It provides a set of simple and easy-to-use APIs to help developers quickly use MCU to access AI models for MCP interaction.

[AiPi-PalChatV1](https://docs.ai-thinker.com/aipi-palchatv1/)'s UART-MCP protocol is a UART-based communication protocol mainly used for MCP interaction between MCU and AI models, with content structure primarily in JSON format. In actual development, developers often need to spend a lot of time handling low-level tasks such as UART serial communication adaptation between MCU and modules, protocol parsing, and data format conversion.

**emMCP** builds a set of simple, easy-to-use, and highly encapsulated APIs (Application Programming Interfaces) that encapsulate all complex low-level operations (such as serial port initialization and configuration, MCP command packaging and parsing, data validation, exception handling, etc.). Developers only need to call the encapsulated interfaces to complete operations like "establishing connection between MCU and module", "sending MCP commands", and "receiving MCP commands", greatly reducing development difficulty and shortening the development cycle.

## Features
- **Support for customizing MCP tools to AI agents**
- **Support for actively waking up Ai-Thinker AI devices**
- **Support for setting communication baud rate, default 115200**
- **Support for setting conversation volume**
- **Built-in small state machine with freely customizable callback functions**
- **Built-in MCP tool management**
- **Memory and FLASH usage (minimum): RAM: 62 Byte, FLASH: 1708 Byte**

## How to Port?

### 1. Download Source Code
It is recommended to use GIT to download the source code with the following command:
```bash
git clone https://github.com/Ai-Thinker-Open/emMCP.git
```

### 2. Import emMCP Library into Your Project
Copy the [port](./port) and [uart-mcp](./uart-mcp) folders to your project, and include the `emMCP.h` file to use all resources of **emMCP**.

> The example provides a CMake example for STM32F103 ([STM32F10xRTOS_MCP](./example/STM32F10xRTOS_MCP)), which you can refer to configure your project.

### 3. Porting Interface

emMCP is a library for serial communication, so you only need to implement the underlying functions for serial communication.

- First, open the `port/uartPort.c` file
- In the function `int uartPortSendData(char *data, int len)`, call your serial port sending function, passing `data` and `len` as parameters. For example:
	```c
	int uartPortSendData(char *data, int len)
	{
		// Implement serial port sending function here
		if (data == NULL || len <= 0)
		{
			return -1;
		}
		return HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 100);
	}
	```
- In the MCU's serial port receiving function, call the receiving function `int uartPortRecvData(char *data, int len)`. For example:
	```c
	void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
	{
		if (huart->Instance == USART2) {
			HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)rxBuffer, sizeof(rxBuffer));
			// Call emMCP receiving function
			uartPortRecvData((char *)rxBuffer, Size);
			__HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
		}
    }
	```
 - Implementation of delay function, in the `port/port.h` file, implement the delay function in the macro `#define emMCP_delay`. For example:
	```c
	#define emMCP_delay osDelay
	```
 - Implementation of memory management functions, in the `port/port.h` file, implement the memory management functions in the macros `#define emMCP_malloc` and `#define emMCP_free`. For example:
	```c
	#define emMCP_malloc malloc
	#define emMCP_free free
	```

### 4. Initialize emMCP

You can initialize emMCP anywhere before executing the main function, for example, before the `main()` function or in a separate initialization function. The code example for initializing emMCP is as follows:
```c
static emMCP_t emMCP_dev;

int main(void)
{
	emMCP_Init(&emMCP_dev);
	while(1)
	{
		// Main loop execution
	}
}
```

### 5. Loop Execution of emMCP_TickHandler()

emMCP has a built-in simple state machine that requires cyclically calling `emMCP_TickHandler()` to process data, reducing the burden when using interrupt functions. It is recommended to call `emMCP_TickHandler()` in the main loop, for example:
```c
static emMCP_t emMCP_dev;

int main(void)
{
	emMCP_Init(&emMCP_dev);
	while(1)
	{
		emMCP_TickHandler(10);
	}
}
```

## For MCP interaction, please refer to [emMCP User Documentation](https://docs.ai-thinker.com/uart_mcp/)

