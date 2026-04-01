/**
 * @file example_main.c
 * @brief emMCP usage example for STM32
 * @version 1.0.0
 */

#include "main.h"
#include "emMCP.h"

/* ========================================================================== */
/* 全局变量                                                                    */
/* ========================================================================== */

static emMCP_t emMCP_dev;
static volatile uint8_t ai_wake_status = 0;

/* ========================================================================== */
/* MCP 工具定义                                                                */
/* ========================================================================== */

/**
 * @brief LED控制工具处理函数
 */
void ledControlHandler(void *args)
{
    cJSON *params = (cJSON *)args;
    cJSON *state = emMCP_GetParam(params, "state");
    
    if (state != NULL && cJSON_IsString(state)) {
        if (strcmp(state->valuestring, "on") == 0) {
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        } else if (strcmp(state->valuestring, "off") == 0) {
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        }
    }
    
    // 发送响应
    emMCP_ResponseValue(emMCP_CTRL_OK);
}

/**
 * @brief 定义LED控制工具
 */
emMCP_tool_t led_tool = {
    .name = "led_control",
    .description = "Control LED on/off state",
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

/* ========================================================================== */
/* 事件回调                                                                    */
/* ========================================================================== */

__emMCPWeak void emMCP_EventCallback(emMCP_event_t event, 
                                      mcp_server_tool_type_t type, 
                                      void *param)
{
    char *param_str = (char *)param;
    
    switch (event) {
        case emMCP_EVENT_AI_WAKE:
            ai_wake_status = 1;
            emMCP_log_info("AI Device Woke Up!");
            break;
            
        case emMCP_EVENT_AI_SLEEP:
            ai_wake_status = 0;
            emMCP_log_info("AI Device Sleeping");
            break;
            
        case emMCP_EVENT_AI_WIFI_CONNECTED:
            emMCP_log_info("WiFi Connected");
            break;
            
        case emMCP_EVENT_AI_WIFI_GOT_IP:
            emMCP_log_info("Got IP Address");
            break;
            
        case emMCP_EVENT_CMD_OK:
            emMCP_log_debug("Command OK: %s", param_str);
            break;
            
        case emMCP_EVENT_CMD_ERROR:
            emMCP_log_error("Command Error: %s", param_str);
            break;
            
        default:
            emMCP_log_debug("Event: %d, Param: %s", event, param_str);
            break;
    }
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void)
{
    // HAL初始化
    HAL_Init();
    SystemClock_Config();
    
    // 外设初始化
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_DMA_Init();
    
    // 启动DMA接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)uart_rx_buffer, sizeof(uart_rx_buffer));
    __HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
    
    // 初始化emMCP
    if (emMCP_Init(&emMCP_dev) != 0) {
        emMCP_log_error("emMCP initialization failed");
        Error_Handler();
    }
    
    // 添加MCP工具
    emMCP_AddToolToToolList(&led_tool);
    
    // 注册工具到AI设备
    emMCP_RegistrationTools();
    
    // 设置音量
    emMCP_SetAiVolume(80);
    
    emMCP_log_info("emMCP initialized successfully");
    
    // 主循环
    while (1) {
        // 处理emMCP事件
        emMCP_TickHandle(10);
        
        // 用户代码...
    }
}

/* ========================================================================== */
/* UART回调                                                                    */
/* ========================================================================== */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2) {
        uartPortRecvData((char *)uart_rx_buffer, Size);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)uart_rx_buffer, sizeof(uart_rx_buffer));
        __HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
    }
}
