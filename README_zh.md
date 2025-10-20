
[![English](https://img.shields.io/badge/Language-English-green.svg)](README.md)
<h1 align="center">emMCP</h1>
<div align="center">

<a href="https://github.com/Ai-Thinker-Open/emMCP/stargazers"><img src="https://img.shields.io/github/stars/Ai-Thinker-Open/emMCP" alt="Stars Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/network/members"><img src="https://img.shields.io/github/forks/Ai-Thinker-Open/emMCP" alt="Forks Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/pulls"><img src="https://img.shields.io/github/issues-pr/Ai-Thinker-Open/emMCP" alt="Pull Requests Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/issues"><img src="https://img.shields.io/github/issues/Ai-Thinker-Open/emMCP" alt="Issues Badge"/></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/graphs/contributors"><img alt="GitHub contributors" src="https://img.shields.io/github/contributors/Ai-Thinker-Open/emMCP?color=2b9348"></a>
<a href="https://github.com/Ai-Thinker-Open/emMCP/blob/master/LICENSE"><img src="https://img.shields.io/github/license/Ai-Thinker-Open/emMCP?color=2b9348" alt="License Badge"/></a>

<i>Like the project?Please consider giving Star ⭐️ to help improve it!</i>

</div>

## 欢迎使用 emMCP

emMCP (Easy mcu MCP) 是一个用于快速开发基于 [小安AI UART-MCP](https://docs.ai-thinker.com/uart_mcp/)的适配库。它提供了一套简单易用的API，帮助开发者快速使用 MCU 接入 AI 模型进行 MCP 交互。

[AiPi-PalChatV1](https://docs.ai-thinker.com/aipi-palchatv1/) 的 UART-MCP 协议是一款基于 UART 的通信协议，主要用于 MCU 和 AI 模型之间进行MCP交互，内容结构主要是 JSON 格式。在实际开发中，开发者往往需要花费大量时间处理 MCU 与模块之间的 UART 串口通信适配、协议解析、数据格式转换等底层工作。

**emMCP** 构建了一套简单易用、高度封装的 API（应用程序编程接口），将复杂的底层操作（如串口初始化配置、MCP 指令打包与解析、数据校验、异常处理等）全部封装起来，开发者只需调用封装好的接口，即可完成 “MCU 与模块建立连接”、“发送 MCP 指令” 和 “接收 MCP 指令” 等操作，大大降低了开发难度，缩短开发周期。

