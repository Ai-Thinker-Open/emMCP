/**
 * @file user_mcp.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include "user_mcp.h"
#include "log.h"
#include "usart.h"
char rxBuffer[MCP_BUFFER_SIZE] = {0};
static emMCP_t emMCP_dev;

emMCP_tool_t led_tool;
emMCP_tool_t air_tool;
static void setLEDRequestHandler(void *arguments) {
  // 获取参数

  cJSON *params = (cJSON *)arguments; // 获取参数
  // 获取被设置参数值
  cJSON *enabled = emMCP_GetParam(params, "enable");
  if (enabled != NULL) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,
                      enabled->valueint ? GPIO_PIN_RESET : GPIO_PIN_SET);
    int ret = emMCP_ResponseValue(emMCP_CTRL_OK); // 返回控制成功
    if (ret != 0) {
      log_error("response error");
    } else {
      log_info("response success");
    }
  } else {
    int ret = emMCP_ResponseValue(emMCP_CTRL_ERROR); // 返回控制失败
    if (ret != 0) {
      log_error("response error");
    } else {
      log_info("response success");
    }
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART2) {
    HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)rxBuffer, sizeof(rxBuffer));
    uartPortRecvData((char *)rxBuffer, Size);
    __HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);
  }
}
void emMCP_EventCallback(emMCP_event_t event, mcp_server_tool_type_t type,
                         void *param) {
  switch (event) {
  case emMCP_EVENT_CMD_OK: {
    log_info("emMCP_EVENT_CMD_OK");

  } break;
  case emMCP_EVENT_CMD_ERROR: {
    log_error("emMCP_EVENT_CMD_ERROR");
  } break;
  case emMCP_EVENT_AI_START: {
    log_info("emMCP_EVENT_AI_START");
  } break;
  case emMCP_EVENT_AI_NETCFG: {
    log_info("emMCP_EVENT_AI_NETCFG");
  } break;
  case emMCP_EVENT_AI_NETERR: {
    log_info("emMCP_EVENT_AI_NETERR");
  } break;
  case emMCP_EVENT_AI_WIFI_CONNNECT: {
    log_info("emMCP_EVENT_AI_WIFI_CONNNECT");
  } break;
  case emMCP_EVENT_AI_WIFI_DISCONNECT: {
    log_info("emMCP_EVENT_AI_WIFI_DISCONNECT");
  } break;
  case emMCP_EVENT_AI_WAKE: {
    log_info("emMCP_EVENT_AI_WAKE");

  } break;
  case emMCP_EVENT_AI_SLEEP: {
    log_info("emMCP_EVENT_AI_SLEEP");
  } break;
  case emMCP_EVENT_AI_MCP_CMD: {
    log_info("emMCP_EVENT_AI_MCP_CMD:%s", (char *)param);

  } break;
  case emMCP_EVENT_AI_MCP_Text:
    log_info("emMCP_EVENT_AI_MCP_Text:%s", (char *)param);
    break;
  default:
    break;
  }
}
void user_mcp_init(void) {

  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer));
  __HAL_DMA_ENABLE_IT(&hdma_usart2_rx, DMA_IT_TC);

  emMCP_Init(&emMCP_dev);
  led_tool.name = "Self.LED.switch";
  led_tool.description = "LED switch tool";
  led_tool.inputSchema.properties[0].name = "enable";
  led_tool.inputSchema.properties[0].description =
      "LED switch, true:on, false:off, when queried, attribute is null";
  led_tool.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
  led_tool.setRequestHandler = setLEDRequestHandler;

  // air_tool.name = "空调开关";
  // air_tool.description = "空调开关工具";
  // air_tool.inputSchema.properties[0].name = "enable";
  // air_tool.inputSchema.properties[0].description = "空调开关，true:on,
  // false:off, when queried, attribute is null";
  // air_tool.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
  int ret = emMCP_AddToolToToolList(&led_tool);
  if (ret != 0) {
    log_error("add tool failed");
    return;
  }
  // ret = emMCP_AddToolToToolList(&air_tool);
  if (ret != 0) {
    log_error("add tool failed");
  }
  ret = emMCP_RegistrationTools();
  if (ret != 0) {
    log_error("MCP registration tools failed");
    return;
  }
  log_info("MCP init success");
}