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

static emMCP_t emMCP_dev;

emMCP_tool_t led_tool;
emMCP_tool_t air_tool;
static void setLEDRequestHandler(void *arguments)
{
	// 获取参数

	cJSON *params = (cJSON *)arguments; // 获取参数
	// 获取被设置参数值
	cJSON *enabled = emMCP_GetParam(params, "enable");
	if (enabled != NULL)
	{
		log_info("set led enabled:%d", enabled->valueint);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, enabled->valueint ? GPIO_PIN_RESET : GPIO_PIN_SET);
		osDelay(10);
		emMCP_ResponseValue(enabled->valueint ? "true" : "false");
	}
	else
	{
		char *value = cJSON_PrintUnformatted(params);
		log_error("get param error, params:%s", value);
		cJSON_free(value);
	}
}

void user_mcp_init(void)
{
	emMCP_Init(&emMCP_dev);
	led_tool.name = "Self.LED.switch";
	led_tool.description = "LED switch tool";
	led_tool.inputSchema.properties[0].name = "enable";
	led_tool.inputSchema.properties[0].description = "LED switch, true:on, false:off, when queried, attribute is null";
	led_tool.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
	led_tool.setRequestHandler = setLEDRequestHandler;
	
	air_tool.name = "空调开关";
	air_tool.description = "空调开关工具";
	air_tool.inputSchema.properties[0].name = "enable";
	air_tool.inputSchema.properties[0].description = "空调开关，true:on, false:off, when queried, attribute is null";
	air_tool.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
	int ret = emMCP_AddToolToToolList(&led_tool);
	if (ret != 0)
	{
		log_error("add tool failed");
		return;
	}
	ret = emMCP_AddToolToToolList(&air_tool);
	if (ret != 0)
	{
		log_error("add tool failed");
	}
	ret = emMCP_RegistrationTools();
	if (ret != 0)
	{
		log_error("MCP registration tools failed");
		return;
	}
	log_info("MCP init success");
}