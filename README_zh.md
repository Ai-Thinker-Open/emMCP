
[![English](https://img.shields.io/badge/Language-English-green.svg)](README.md)
<h1 align="center">emMCP</h1>
<div align="center">

<a href="https://github.com/Ai-Thinker-Open/emMCP/stargazers"><img src="https://img.shields.io/github/stars/Ai-Thinker-Open/emMCP" alt="Stars Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/network/members"><img src="https://img.shields.io/github/forks/Ai-Thinker-Open/emMCP" alt="Forks Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/pulls"><img src="https://img.shields.io/github/issues-pr/Ai-Thinker-Open/emMCP" alt="Pull Requests Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/issues"><img src="https://img.shields.io/github/issues/Ai-Thinker-Open/emMCP" alt="Issues Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/graphs/contributors"><img alt="GitHub contributors" src="https://img.shields.io/github/contributors/Ai-Thinker-Open/emMCP?color=2b9348"></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/blob/master/LICENSE"><img src="https://img.shields.io/github/license/Ai-Thinker-Open/emMCP?color=2b9348" alt="License Badge"/></a>

<i>喜欢该项目吗？请考虑点个 Star ⭐️ ，以帮助改进该项目！</i>

</div>

## 欢迎使用 emMCP

emMCP (Easy mcu MCP) 是一个用于快速开发基于 [小安AI UART-MCP](https://docs.ai-thinker.com/uart_mcp/)的适配库。它提供了一套简单易用的API，帮助开发者快速使用 MCU 接入 AI 模型进行 MCP 交互。

[AiPi-PalChatV1](https://docs.ai-thinker.com/aipi-palchatv1/) 的 UART-MCP 协议是一款基于 UART 的通信协议，主要用于 MCU 和 AI 模型之间进行MCP交互，内容结构主要是 JSON 格式。在实际开发中，开发者往往需要花费大量时间处理 MCU 与模块之间的 UART 串口通信适配、协议解析、数据格式转换等底层工作。

**emMCP** 构建了一套简单易用、高度封装的 API（应用程序编程接口），将复杂的底层操作（如串口初始化配置、MCP 指令打包与解析、数据校验、异常处理等）全部封装起来，开发者只需调用封装好的接口，即可完成 “MCU 与模块建立连接”、“发送 MCP 指令” 和 “接收 MCP 指令” 等操作，大大降低了开发难度，缩短开发周期。

## 功能特点
- **支持自主定义 MCP 工具到 AI智能体**
- **支持主动唤醒小安AI设备**
- **支持设置通讯波特率，默认 115200**
- **支持设置对话音量**
- **内置小型状态机，可自由回调函数**
- **自带MCP工具管理**
- **内存及FLASH使用情况（最小占用）：RAM:62 Byte，FLAHS: 1708 Byte** 

## 如何移植？

### 1. 下载源码
建议使用 GIT 下载源码，命令如下：
```bash
git clone https://github.com/Ai-Thinker-Open/emMCP.git
```
### 2. 在项目中引入 emMCP 库
将 [port](./port) 和 [uart-mcp](./uart-mcp) 文件夹复制到你的项目当中，并引用 `emMCP.h` 文件使用 **emMCP** 的所有资源。

>示例中提供了 STM32F103 的 CMake 示例（[STM32F10xRTOS_MCP](./example/STM32F10xRTOS_MCP)），你可以参考它来配置你的项目。

### 3. 移植接口

emMCP 是一个适用于串口通讯的库，因此你只需要实现串口通讯的底层函数。

- 首先打开 `port/uartPort.c` 文件
- 在函数`int uartPortSendData(char *data, int len)` 当中调用你的串口发送函数，将 `data` 和 `len` 作为参数传入。例如：
	```c
	int uartPortSendData(char *data, int len)
	{
		// 在此处实现串口发送函数
		if (data == NULL || len <= 0)
		{
			return -1;
		}
		return HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 100);
	}
	```
- 在 MCU 的串口接收函数当中调用 `int uartPortRecvData(char *data, int len)` 的接收函数，例如：
	```c
	void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) 
	{
		if (huart->Instance == USART2) {
			HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)rxBuffer, sizeof(rxBuffer));
			//调用emMCP接收函数
			uartPortRecvData((char *)rxBuffer, Size);
			__HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
		}
    }
	```
 - 延时函数的实现，在 `port/port.h` 文件中，将延时函数在宏`#define emMCP_delay` 中实现，例如：
	```c
	#define emMCP_delay osDelay
	```
 - 内存管理函数的实现，在 `port/port.h` 文件中，将内存管理函数在宏`#define emMCP_malloc` 和 `#define emMCP_free` 中实现，例如：
	```c
	#define emMCP_malloc malloc
	#define emMCP_free free
	```
### 4. 初始化 emMCP

你可在任何未执行主函数的地方初始化 emMCP，例如在 `main()` 函数之前，或者在一个单独的初始化函数中。初始化 emMCP 的代码示例如下：
```c
static emMCP_t emMCP_dev;

int main(void)
{
	emMCP_Init(&emMCP_dev);
	whiet(1)
	{
		//主循环执行
	}
}
```

### 5. 循环执行 emMCP_TickHandler()

emMCP 内置了一个简单的状态机，需要循环地调用 `emMCP_TickHandler()` 处理数据，以减轻当使用中断函数时的负担。推荐在主循环中调用 `emMCP_TickHandler()`，例如：
```c
static emMCP_t emMCP_dev;

int main(void)
{
	emMCP_Init(&emMCP_dev);
	whiet(1)
	{
		emMCP_TickHandler(10);
	}
}
```
## MCP 交互请参考 [emMCP使用文档](https://docs.ai-thinker.com/uart_mcp/)